#include "Interpreter.h"

#include <iostream>




Interpreter::Interpreter()
	: m_head(0)
	, m_ptr(0)
	, m_register(0)
{
	clear();
}

//######################################################################################################

void Interpreter::ready(const std::string& code)
{
	clear();


	Modes mode = Modes::FindBeginning;
	Command tempCmd;
	bool isNegative = false;

	const auto codeLength = code.length();
	for (std::size_t codeIndex = 0; codeIndex < codeLength; )
	{
		std::string token;
		token.push_back(code[codeIndex]);
		if (codeIndex < codeLength - 1)
			token.push_back(code[codeIndex + 1]);

		switch (mode)
		{
		case Modes::FindBeginning:
			codeIndex += parseCommandBeginning(token, tempCmd, mode);
			break;

		case Modes::ParseFunction:
			codeIndex += parseCommandFunction(token, tempCmd, mode);
			break;

		case Modes::ParseConstant:
			codeIndex += parseCommandConstant(token, tempCmd, mode,
				isNegative);
			break;

		case Modes::ParseJob:
			codeIndex += parseCommandJob(token, tempCmd, mode);
			break;
		}
	}

	if (mode == Modes::ParseConstant)
	{
		parseCommandConstant("eof", tempCmd, mode,
			isNegative);
	}

	if (mode == Modes::ParseJob)
	{
		parseCommandJob("eof", tempCmd, mode);
	}
}


void Interpreter::execute()
{
	while (executeStepwise());
}


bool Interpreter::executeStepwise()
{
	if (m_head >= m_code.size())
		return false;


	const auto& cmd = m_code[m_head];

	int returnValue = runFuction(cmd);
	runWrite(cmd, returnValue);
	runJob(cmd, returnValue);


	return (m_head < m_code.size());
}

//######################################################################################################

void Interpreter::clear()
{
	m_code.clear();
	m_head = 0;
	m_ptr = 0;
	m_ram.clear();
	m_ram.resize(32, 0);
	while (!m_stack.empty())
		m_stack.pop();
	m_register = 0;
}

//######################################################################################################

std::size_t Interpreter::parseCommandBeginning(const std::string& str, Command& outCmd, Modes& mode)
{
	std::size_t jumpIndex = 1;
	bool isFunction = false;


	if (str.find("¿ì") == 0)
	{
		outCmd.write = Command::WriteTo::Stack;
	}
	else if (str.find("À¸") == 0)
	{
		outCmd.write = Command::WriteTo::Ram;
	}
	else if (str.find("¿ò") == 0)
	{
		outCmd.write = Command::WriteTo::Register;
	}
	else
	{
		mode = Modes::ParseFunction;
		std::size_t jmp = parseCommandFunction(str, outCmd, mode);

		if (mode != Modes::FindBeginning)
		{
			outCmd.write = Command::WriteTo::Nothing;
			jumpIndex = jmp;

			isFunction = true;
		}
	}


	if (outCmd.write != Command::WriteTo::None)
	{
		if (isFunction == false)
		{
			mode = Modes::ParseFunction;
			jumpIndex = 2;
		}
	}


	return jumpIndex;
}


std::size_t Interpreter::parseCommandFunction(const std::string& str, Command& outCmd, Modes& mode)
{
	std::size_t jumpIndex = 2;


	if (str.find("¸Ï") == 0)
	{
		outCmd.func = Command::Functions::ReturnConstant;

		mode = Modes::ParseConstant;
	}
	else if (str.find("¸Ò") == 0)
	{
		outCmd.func = Command::Functions::ReturnRam;
	}
	else if (str.find("Ñ") == 0)
	{
		outCmd.func = Command::Functions::ReturnStack;
	}
	else if (str.find("Î") == 0)
	{
		outCmd.func = Command::Functions::ReturnRegister;
	}
	else if (str.find("Õ") == 0)
	{
		outCmd.func = Command::Functions::ReturnInput;
	}
	else
	{
		mode = Modes::FindBeginning;
		jumpIndex = 0;

		outCmd = Command{};
	}

	if (mode == Modes::ParseFunction)
	{
		mode = Modes::ParseJob;
	}


	return jumpIndex;
}


std::size_t Interpreter::parseCommandConstant(const std::string& str, Command& outCmd, Modes& mode,
	bool& isNegative)
{
	std::size_t jumpIndex = 2;


	if (str.find("¸Ï") == 0)
	{
		outCmd.constant = ((outCmd.constant << 1) | 1);
	}
	else if (str.find("¾Æ") == 0)
	{
		if (outCmd.constant == 0)
		{
			isNegative = true;
		}
		else
		{
			outCmd.constant = (outCmd.constant << 1);
		}
	}
	else
	{
		if (isNegative)
		{
			outCmd.constant = -outCmd.constant;

			isNegative = false;
		}


		mode = Modes::ParseJob;
		jumpIndex = 0;
	}


	return jumpIndex;
}


std::size_t Interpreter::parseCommandJob(const std::string& str, Command& outCmd, Modes& mode)
{
	std::size_t jumpIndex = 1;

	
	auto job = Command::Jobs::None;

	if (str.find("!") == 0)
	{
		job = Command::Jobs::JumpIfNotEquals;
	}
	else if (str.find("?") == 0)
	{
		job = Command::Jobs::JumpIfEquals;
	}
	else if (str.find("~") == 0)
	{
		job = Command::Jobs::AddPointer;
	}
	else if (str.find("-") == 0)
	{
		job = Command::Jobs::AddHeader;
	}
	else if (str.find(".") == 0)
	{
		job = Command::Jobs::AddPointerData;
	}
	else if (str.find(",") == 0)
	{
		job = Command::Jobs::Output;
	}
	else if (str.find(";") == 0)
	{
		job = Command::Jobs::MoveHeader;
	}
	else
	{
		if (outCmd.jobs.empty())
		{
			outCmd.jobs.push_back(Command::Jobs::Nothing);
		}

		if (outCmd.func != Command::Functions::None)
		{
			m_code.emplace_back(std::move(outCmd));
		}
		outCmd = Command{};

		mode = Modes::FindBeginning;
		jumpIndex = 0;
	}

	if (job != Command::Jobs::None)
	{
		outCmd.jobs.push_back(job);
	}


	return jumpIndex;
}

//######################################################################################################

int Interpreter::runFuction(const Command& cmd)
{
	switch (cmd.func)
	{
	case Command::Functions::ReturnConstant:
		return cmd.constant;
		break;

	case Command::Functions::ReturnRam:
		return m_ram[m_ptr];
		break;

	case Command::Functions::ReturnStack:
		if (m_stack.empty())
		{
			return 0;
		}
		else
		{
			auto bak = m_stack.top();
			m_stack.pop();
			return bak;
		}
		break;

	case Command::Functions::ReturnRegister:
		return m_register;
		break;

	case Command::Functions::ReturnInput:
		return std::cin.get();
		break;

	default:
		std::cerr << "Unknown Function!";
		break;
	}


	return 0;
}


void Interpreter::runWrite(const Command& cmd, int data)
{
	switch (cmd.write)
	{
	case Command::WriteTo::Nothing:
		break;

	case Command::WriteTo::Register:
		m_register = data;
		break;

	case Command::WriteTo::Ram:
		m_ram[m_ptr] = data;
		break;

	case Command::WriteTo::Stack:
		m_stack.push(data);
		break;

	default:
		std::cerr << "Unknown Write!";
		break;
	}
}


void Interpreter::runJob(const Command& cmd, int data)
{
	bool autoMove = true;

	for (auto& job : cmd.jobs)
	{
		switch (job)
		{
		case Command::Jobs::Nothing:
			break;

		case Command::Jobs::Output:
			std::cout.put(data);
			break;

		case Command::Jobs::JumpIfNotEquals:
			if (data != m_register)
			{
				++m_head;
			}
			break;

		case Command::Jobs::JumpIfEquals:
			if (data == m_register)
			{
				++m_head;
			}
			break;

		case Command::Jobs::AddPointer:
			if (data >= 0)
			{
				for (int i = 0; i < data; ++i)
				{
					incPtr();
				}
			}
			else
			{
				for (int i = data; i < 0; ++i)
				{
					decPtr();
				}
			}
			break;

		case Command::Jobs::AddHeader:
			autoMove = false;

			if (data >= 0)
			{
				m_head += static_cast<decltype(m_head)>(data);
			}
			else
			{
				auto absData = static_cast<decltype(m_head)>(-data);
				if (m_head >= absData)
				{
					m_head -= absData;
				}
				else
				{
					m_head = 0;
				}
			}
			break;

		case Command::Jobs::AddPointerData:
			m_ram[m_ptr] += data;
			break;

		case Command::Jobs::MoveHeader:
			autoMove = false;

			if (data >= 0)
			{
				m_head = static_cast<decltype(m_head)>(data);
			}
			else
			{
				auto absData = static_cast<decltype(m_head)>(-data);
				if (m_code.size() >= absData)
				{
					m_head = m_code.size() - absData;
				}
				else
				{
					m_head = 0;
				}
			}
			break;

		default:
			std::cerr << "Unknown Job!";
			break;
		}
	}

	if (autoMove)
	{
		++m_head;
	}
}


void Interpreter::incPtr()
{
	++m_ptr;
	if (m_ptr >= m_ram.size())
	{
		m_ram.push_back(0);
	}
}


void Interpreter::decPtr()
{
	if (m_ptr > 0)
	{
		--m_ptr;
	}
	else
	{
		m_ram.push_front(0);
		m_ptr = 0;
	}
}

