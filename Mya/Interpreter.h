#ifndef __NEUROWHAI__MYA__INTERPRETER_H__
#define __NEUROWHAI__MYA__INTERPRETER_H__


#include <vector>
#include <string>
#include <deque>
#include <stack>


class Interpreter
{
private:
	class Command
	{
	public:
		enum class WriteTo
		{
			None,
			Nothing,
			Register,
			Ram,
			Stack,
		};

		enum class Functions
		{
			None,
			ReturnConstant,
			ReturnRam,
			ReturnStack,
			ReturnRegister,
			ReturnInput,
		};

		enum class Jobs
		{
			None,
			Nothing,
			Output,
			JumpIfNotEquals,
			JumpIfEquals,
			AddPointer,
			AddHeader,
			AddPointerData,
			MoveHeader,
		};

	public:
		WriteTo write = WriteTo::None;
		Functions func = Functions::None;
		int constant = 0;
		std::deque<Jobs> jobs;
	};
	
	enum class Modes
	{
		FindBeginning,
		ParseFunction,
		ParseConstant,
		ParseJob,
	};


public:
	Interpreter();


private:
	std::vector<Command> m_code;
	std::size_t m_head;
	std::size_t m_ptr;
	std::deque<int> m_ram;
	std::stack<int> m_stack;
	int m_register;


public:
	void ready(const std::string& code);
	void execute();
	bool executeStepwise();


private:
	void clear();


private:
	std::size_t parseCommandBeginning(const std::string& str, Command& outCmd, Modes& mode);
	std::size_t parseCommandFunction(const std::string& str, Command& outCmd, Modes& mode);
	std::size_t parseCommandConstant(const std::string& str, Command& outCmd, Modes& mode,
		bool& isNegative);
	std::size_t parseCommandJob(const std::string& str, Command& outCmd, Modes& mode);


private:
	int runFuction(const Command& cmd);
	void runWrite(const Command& cmd, int data);
	void runJob(const Command& cmd, int data);
	void incPtr();
	void decPtr();
};


#endif