#include "llvm.hpp"

#include "codegen.hpp"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <stack>

namespace bf::codegen
{

// Strongly inspired from https://github.com/jeremyroman/llvm-brainfuck

struct Loop
{
	llvm::BasicBlock *entry, *body, *exit;
	llvm::PHINode *data_ptr_body, *data_ptr_exit;
};

bool llvm(Context ctx)
{
	using namespace llvm;
	LLVMContext llvm_context;
	Module main_module{"brainfuck program", llvm_context};

	IRBuilder builder{llvm_context};

	Type* cell_type = Type::getInt8Ty(llvm_context);

	ArrayType* memory_type = ArrayType::get(cell_type, 30000);

	GlobalVariable* memory = new GlobalVariable(
		main_module,
		memory_type,
		false,
		GlobalVariable::InternalLinkage,
		Constant::getNullValue(memory_type),
		"data"
	);

	Value* data_ptr = builder.CreateConstInBoundsGEP2_32(
		nullptr,
		memory,
		0,
		0,
		"ptr"
	);

	FunctionType* cout_function_type = FunctionType::get(
		Type::getVoidTy(llvm_context),
		std::array<Type*, 1>{cell_type},
		false
	);

	Function* cout_function = Function::Create(
		cout_function_type,
		Function::ExternalLinkage,
		"brainfuck_put",
		main_module
	);

	FunctionType* main_func_type = FunctionType::get(
		Type::getVoidTy(llvm_context),
		{},
		false
	);

	Function* main_func = Function::Create(
		main_func_type,
		Function::ExternalLinkage,
		"brainfuck_main",
		&main_module
	);

	BasicBlock* main_block = BasicBlock::Create(
		llvm_context,
		"entry",
		main_func
	);

	builder.SetInsertPoint(main_block);

	Loop current_loop;
	std::stack<Loop> loops;

	for (std::size_t i = 0; i < ctx.program.size(); ++i)
	{
		const auto& op = ctx.program[i];
		switch (op.opcode)
		{
		case Opcode::bfAdd:
		{
			builder.CreateStore(
				builder.CreateAdd(
					builder.CreateLoad(data_ptr),
					ConstantInt::get(cell_type, op.args[0])
				),
				data_ptr
			);

			break;
		}

		case Opcode::bfAddOffset:
		{
			auto* offset_ptr = builder.CreateConstGEP1_32(data_ptr, op.args[1], "tmp_ptr");

			builder.CreateStore(
				builder.CreateAdd(
					builder.CreateLoad(offset_ptr),
					ConstantInt::get(cell_type, op.args[0])
				),
				offset_ptr
			);

			break;
		}

		case Opcode::bfShift:
		{
			data_ptr = builder.CreateConstGEP1_32(data_ptr, op.args[0], "ptr");
			break;
		}

		case Opcode::bfMAC:
		{
			auto* offset_ptr = builder.CreateConstGEP1_32(data_ptr, op.args[1], "tmp_mac_ptr");

			builder.CreateStore(
				builder.CreateAdd(
					builder.CreateLoad(data_ptr),
					builder.CreateMul(
						ConstantInt::get(cell_type, op.args[0]),
						builder.CreateLoad(offset_ptr)
					)
				),
				data_ptr
			);

			break;
		}

		case Opcode::bfCharOut:
		{
			builder.CreateCall(
				cout_function,
				builder.CreateLoad(data_ptr)
			);
			break;
		}

		case Opcode::bfCharIn:
		{
			warnout(codegenllvminfo) << "Skipping I/O op.\n";
			break;
		}

		case Opcode::bfJmpZero:
		case Opcode::bfJmpNotZero:
		{
			errout(codegenllvminfo) << "Unexpected conditional jump. LLVM IR compilation must occur before BF linking.\n";
			return false;
		}

		case Opcode::bfSet:
		{
			builder.CreateStore(
				ConstantInt::get(cell_type, op.args[0]),
				data_ptr
			);
			break;
		}

		case Opcode::bfSetOffset:
		{
			auto* offset_ptr = builder.CreateConstGEP1_32(data_ptr, op.args[1], "tmp_ptr");

			builder.CreateStore(
				ConstantInt::get(cell_type, op.args[0]),
				offset_ptr
			);

			break;
		}

		case Opcode::bfLoopBegin:
		{
			current_loop.entry = builder.GetInsertBlock();
			current_loop.body = BasicBlock::Create(llvm_context, "loop", main_func);
			current_loop.exit = BasicBlock::Create(llvm_context, "exit", main_func);

			builder.CreateCondBr(
				builder.CreateIsNotNull(
					builder.CreateLoad(data_ptr)
				),
				current_loop.body,
				current_loop.exit
			);

			builder.SetInsertPoint(current_loop.exit);
			current_loop.data_ptr_exit = builder.CreatePHI(data_ptr->getType(), 2, "ptr");
			current_loop.data_ptr_exit->addIncoming(data_ptr, current_loop.entry);

			builder.SetInsertPoint(current_loop.body);
			current_loop.data_ptr_body = builder.CreatePHI(data_ptr->getType(), 2, "ptr");
			current_loop.data_ptr_body->addIncoming(data_ptr, current_loop.entry);

			data_ptr = current_loop.data_ptr_body;

			loops.push(current_loop);
			break;
		}

		case Opcode::bfLoopEnd:
		{
			if (loops.empty())
			{
				errout(codegenllvminfo) << "Unexpected ']': missing '[' while generating IR\n";
				return false;
			}

			current_loop = loops.top();
			loops.pop();

			current_loop.data_ptr_body->addIncoming(data_ptr, builder.GetInsertBlock());
			current_loop.data_ptr_exit->addIncoming(data_ptr, builder.GetInsertBlock());

			builder.CreateCondBr(
				builder.CreateIsNotNull(
					builder.CreateLoad(data_ptr)
				),
				current_loop.body,
				current_loop.exit
			);

			current_loop.exit->moveAfter(builder.GetInsertBlock());
			data_ptr = current_loop.data_ptr_exit;
			builder.SetInsertPoint(current_loop.exit);

			break;
		}

		case Opcode::bfShiftUntilZero:
		{
			errout(codegenllvminfo) << "Unexpected SUZ opcode. The LLVM IR target requires -optimize-suz=0.\n";
			return false;
		}

		case Opcode::bfEnd:
		{
			builder.CreateRetVoid();
			break;
		}

		default:
			errout(codegenllvminfo) << "Unhandled opcode: " << int(op.opcode) << '\n';
			return false;
		}
	}

	if (!loops.empty())
	{
		errout(codegenllvminfo) << "Unexpected '[': missing ']' while finishing IR\n";
		return false;
	}

	FunctionPassManager pass_manager;
	pass_manager.addPass(InstCombinePass());

	FunctionAnalysisManager analysis_manager;
	LoopAnalysisManager loop_analysis_manager;

	PassBuilder pass_builder;
	pass_builder.registerFunctionAnalyses(analysis_manager);
	pass_builder.registerLoopAnalyses(loop_analysis_manager);

	pass_manager.run(*main_func, analysis_manager);

	main_module.print(outs(), nullptr);

	return true;
}

}
