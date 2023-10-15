#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>

using namespace std;

class Employee
{
private:
	static int id_generator;

public:
	int id;
	string name = "", type = "Employee";
	Employee() : id(++id_generator) {}
	Employee(int id_0) : id(id_0) 
	{
		id_generator = id_0;
	}
	Employee(string name) : id(++id_generator), name(name) {}

	virtual ~Employee() = default;

	virtual int CalculateSalary() const
	{
		return 0;
	}

	// ask the employee's name and the salary of this employee in derived classes
	virtual void AskSalary()
	{
		cout << "Please enter employee name: ";
		getline(cin, name);
	}

	// convert data to string with a separator symbol from method's parameter
	virtual string ToString(char sep) 
	{
		return type + sep + to_string(id) + sep + name + sep + to_string(CalculateSalary());
	}

	// print all employees in the list
	void print(ostream& os) const
	{
		os << "Employee Payroll\n" << "================\n" << "Payroll for: " << id << " - " << name << "\n"
			<< "- Check amount: " << CalculateSalary() << "\n\n";
	}
};

int Employee::id_generator = 0;

class SalaryEmployee : public Employee
{
	int monthly_salary = 0;

public:
	// here is the type by which the employee's salary type from the file is later determined
	string type = "S";

	SalaryEmployee() = default;
	SalaryEmployee(int id, string name, int monthly_salary) : Employee(name), monthly_salary(monthly_salary) {}

	void AskSalary() override
	{
		Employee::AskSalary();
		cout << "Please enter monthly salary: ";
		cin >> monthly_salary;
		cin.ignore();
	}

	int CalculateSalary() const override
	{
		return monthly_salary;
	}

	string ToString(char sep) override 
	{
		return to_string(id) + sep + name + sep + type + sep + to_string(monthly_salary);
	}
};

class HourlyEmployee : public Employee
{
	int hour_rate = 0, hours_worked = 0;

public:
	string type = "H";

	HourlyEmployee() = default;
	HourlyEmployee(int id, string name, int hour_rate1, int hours_worked1) : Employee(name),
		hour_rate(hour_rate1), hours_worked(hours_worked1) {}

	void AskSalary() override
	{
		Employee::AskSalary();
		cout << "Please enter hour rate: ";
		cin >> hour_rate;
		cout << "Please enter hours worked: ";
		cin >> hours_worked;
		cin.ignore();
	}

	int CalculateSalary() const override
	{
		return hour_rate * hours_worked;
	}

	string ToString(char sep) override 
	{
		return to_string(id) + sep + name + sep + type + sep + to_string(hour_rate) + sep
			+ to_string(hours_worked);
	}
};

class CommissionEmployee : public SalaryEmployee
{
	int commission = 0;

public:
	string type = "C";

	CommissionEmployee() = default;
	CommissionEmployee(int id, string name, int monthly_salary, int commission1) :
		SalaryEmployee(id, name, monthly_salary), commission(commission1) {}

	void AskSalary() override
	{
		SalaryEmployee::AskSalary();
		cout << "Please enter commission: ";
		cin >> commission;
		cin.ignore();
	}

	int CalculateSalary() const override
	{
		return SalaryEmployee::CalculateSalary() + commission;
	}
	string ToString(char sep) override 
	{
		return to_string(id) + sep + name + sep + type + sep +
			to_string(SalaryEmployee::CalculateSalary()) + sep + to_string(commission);
	}
};

class PayrollSystem
{
	vector<Employee*> employees;
public:

	PayrollSystem() = default;
	~PayrollSystem()
	{
		for (auto e : employees)
		{
			delete e;
		}
	}
	// this is a user interface where we can choose what we want to do
	void menu()
	{
		int value = 9;
		constexpr int QUIT = 0;

		while (value != QUIT)
		{
			cout << "Payroll menu\n";
			cout << "============\n";
			cout << "(1) Add employees\n";
			cout << "(2) Write employees to file\n";
			cout << "(3) Read employees from file\n";
			cout << "(4) Print payroll\n";
			cout << "(0) Quit\n";
			cout << "Please select: ";
			cin >> value;
			cout << '\n';
			switch (value)
			{
			case 1:
			{
				// this is a user interface where we can choose a salary type and write it
				while (value != QUIT)
				{
					cout << "Salary type\n";
					cout << "-----------\n";
					cout << "(1) Monthly\n";
					cout << "(2) Hourly\n";
					cout << "(3) Commission\n";
					cout << "(0) Quit\n";
					cout << "Please enter salary type: ";
					cin >> value;
					cout << '\n';
					cin.ignore();
					Employee* employee = nullptr;
					switch (value)
					{
					case 1:
					{
						employee = new SalaryEmployee;
						break;
					}
					case 2:
					{
						employee = new HourlyEmployee;
						break;
					}
					case 3:
					{
						employee = new CommissionEmployee;
						break;
					}
					case 0:
					{
						break;
					}
					default:
					{
						cout << "Please write a correct number (0-3)\n";
					}
					}
					if (employee)
					{
						employee->AskSalary();
						/* here is a function that compares the names in the list and the name
							we wrote regardless of the class type */
						auto compare = [employee](Employee* e) { return e->name == employee->name; };
						auto i = std::find_if(employees.begin(), employees.end(), compare);
						if (i != employees.end())
						{
							cout << "This employee is here!\n";
							delete employee;
						}
						else 
						{
							employees.push_back(employee);
						}
					}
				}
				value = 9;
				break;
			}
			case 2:
				writetofile(employees);
				break;
			case 3:
				readfromfile();
				break;
			case 4:
				print(cout);
				break;
			case 0:
				break;
			default:
				cout << "incorrect selection\n";
				break;
			}
		}
	}

	void print(ostream& os)
	{
		for (auto e : employees)
		{
			e->print(os);
		}
	}

	// here is a function that divides a sentence into words by delimiter characters
	vector<string> mysplit(string sentence, char sep)
	{
		vector<string> vec;
		size_t start;
		size_t end = 0;

		while ((start = sentence.find_first_not_of(sep, end)) != string::npos)
		{
			end = sentence.find(sep, start);
			vec.push_back(sentence.substr(start, end - start));
		}
		return vec;
	}

	// here is a function that words into a sentence through a delimiter character
	vector<string> myjoin(char sep)
	{
		vector<string> str1;
		for (auto e : employees)
		{
			str1.emplace_back(e->ToString(','));
		}
		return str1;
	}

	void writetofile(vector<Employee*> employees)
	{
		int counter = 0;
		vector<Employee*> employees1 = employees;
		fstream file("employee.csv", ios_base::out | ios_base::app);
		if (file.is_open())
		{
			auto str = myjoin(',');
			readfromfile(false);
			for (int i = 0; i < employees1.size(); i++)
			{
				// here we also compare the names in the file and in the employees vector
				Employee* empl = employees1[i];
				auto compare1 = [empl](Employee* ee) { return ee->name == empl->name; };
				auto x = std::find_if(this->employees.begin(), this->employees.end(), compare1);
				if (x == this->employees.end() || this->employees.size() == 0)
				{
					file << str[i] << '\n';
					counter++;
				}
			}
		}
		cout << counter << " employee(s) added to file " << "employee.csv" << endl;
		file.close();
		readfromfile(false);
	}

	void readfromfile(bool a = true)
	{
		employees.clear();
		Employee(0);
		int id = 0, salary = 0;
		string tmp, name;
		ifstream file("employee.csv");
		if (file.is_open())
		{
			while (!file.eof())
			{
				getline(file, tmp);
				if (tmp == "")
				{
					break;
				}
				vector<string> temp = mysplit(tmp, ',');

				if (temp[2] == "S")
				{
					id = stoi(temp[0]);
					name = temp[1];
					salary = stoi(temp[3]);
					employees.push_back(new SalaryEmployee(id, name, salary));
				}
				else if (temp[2] == "H")
				{
					id = stoi(temp[0]);
					name = temp[1];
					employees.push_back(new HourlyEmployee(id, name, stoi(temp[3]), stoi(temp[4])));
				}
				else if (temp[2] == "C")
				{
					id = stoi(temp[0]);
					name = temp[1];
					salary = stoi(temp[3]);
					employees.push_back(new CommissionEmployee(id, name, salary, stoi(temp[4])));
				}
			}
		}
		/* Due to the necessity of reading data from a file when launching the program,
		a condition for outputting the message was added */
		if (a)
		{
			cout << employees.size() << " employee(s) read from file " << "employee.csv" << endl;
		}
	}
};

int main()
{
	PayrollSystem mylist;
	mylist.readfromfile(false);
	mylist.menu();
	return 0;
}