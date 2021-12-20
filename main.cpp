#include <iostream>
#include <fstream>
#include <ostream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>
#include <map>
#include <iomanip>

int main(int argc, char **argv)
{
	if (argc != 6)
	{
		std::cout << "Usage: " << argv[0] << " <courses.csv>"
				  << " <students.csv>"
				  << " <tests.csv>"
				  << " <marks.csv>"
				  << " <results.json>" << std::endl;
		return 0;
	}
	// Opens files and creates storage for them
	std::ifstream courses(argv[1]);
	std::ifstream students(argv[2]);
	std::ifstream tests(argv[3]);
	std::ifstream marks(argv[4]);
	std::string line = "";
	std::string courses_line = "";
	std::string students_line = "";
	std::string tests_line = "";
	std::string marks_line = "";

	// Reads in courses
	getline(courses, line);
	std::map<int, std::pair<std::string, std::string>> cou;
	while (getline(courses, line))
	{
		std::string tmp = "";
		int id = 0;
		std::string name = "";
		std::string teacher = "";
		int delim = 1;
		for (int i = 0; i < line.length(); i++)
		{
			if (line[i] != ',')
			{
				tmp += line[i];
			}
			if (line[i] == ',' && delim == 1)
			{
				id = std::stoi(tmp);
				tmp = "";
				delim++;
			}
			if (line[i] == ',' && delim == 2)
			{
				name = tmp;
				tmp = "";
			}
			if (i + 1 == line.length())
			{
				teacher = tmp;
				std::pair<std::string, std::string> nate(name, teacher);
				cou.insert(std::pair<int, std::pair<std::string, std::string>>(id, nate));
			}
		}
		courses_line += line;
	}
	courses.close();

	// Reads in students
	std::map<int, std::string> stu;
	// Skips first line
	getline(students, line);
	while (getline(students, line))
	{
		std::string tmp = "";
		int id = 0;
		for (int i = 0; i < line.length(); i++)
		{
			if (line[i] != ',')
			{
				tmp += line[i];
			}
			if (line[i] == ',')
			{
				id = std::stoi(tmp);
				tmp = "";
			}
			if (i + 1 == line.length())
			{
				stu.insert(std::pair<int, std::string>(id, tmp));
				tmp = "";
			}
		}
	}
	students.close();

	// Reads in tests
	std::map<int, std::pair<int, int>> tes;
	getline(tests, line);
	while (getline(tests, line))
	{
		std::string tmp = "";
		int id = 0;
		int course_id = 0;
		int weight = 0;
		int delim = 1;
		for (int i = 0; i < line.length(); i++)
		{
			if (line[i] != ',')
			{
				tmp += line[i];
			}
			if (line[i] == ',' && delim == 1)
			{
				id = std::stoi(tmp);
				tmp = "";
				delim++;
				continue;
			}
			if (line[i] == ',' && delim == 2)
			{
				course_id = std::stoi(tmp);
				tmp = "";
				continue;
			}
			if (i + 1 == line.length())
			{
				weight = std::stoi(tmp);
				std::pair<int, int> cowe(course_id, weight);
				tes.insert(std::pair<int, std::pair<int, int>>(id, cowe));
			}
		}
	}
	tests.close();
	// Keeps track of unique test_id in order to sum the weight correctly
	int t = tes.begin()->second.first;
	int sum = 0;
	// Will scan tes map (test map) to make sure all weights add up to 100
	// If weights do not add up to 100 will output a json file with error output
	for (auto i = tes.begin(); i != tes.end(); i++)
	{
		if (i->second.first == t)
		{
			sum += i->second.second;
		}
		else
		{
			if (sum != 100)
			{

				std::ofstream output(argv[5]);
				output << "{\n";
				output << "\"error\": \"Invalid Course Weights\"\n";
				output << "}";
				output.close();
				return 0;
			}
			sum = 0;
			t = i->second.first;
			sum = i->second.second;
		}
	}

	// Reads in marks
	marks >> line;
	// int is student id, pair is course_id, average
	// std::map<int, std::pair<int, float>> courseAverage;
	int student_id = 1;
	int curr_student_id = 1;
	int test_id = 1;
	int curr_test_id = 1;
	int course_id = tes.find(1)->second.first;
	int curr_course_id = tes.find(1)->second.first;
	float mark = 0.00;
	int delim = 1;
	float avg = 0.00;
	// student_id, course_id, avg
	std::vector<std::tuple<int, int, float>> coursesStudentsTook;
	while (marks >> line)
	{
		std::string tmp = "";
		for (int i = 0; i < line.length(); i++)
		{
			if (line[i] != ',')
			{
				tmp += line[i];
			}
			if (line[i] == ',' && delim == 1)
			{
				// sets test_id
				test_id = std::stoi(tmp);
				tmp = "";
				delim++;
				continue;
			}
			if (line[i] == ',' && delim == 2)
			{
				// sets student_id
				student_id = std::stoi(tmp);
				tmp = "";
				delim = 1;
				continue;
			}
			if (i + 1 == line.length())
			{
				// sets mark
				mark = std::stoi(tmp);
				tmp = "";
			}
		}
		coursesStudentsTook.push_back(std::make_tuple(student_id, test_id, (mark * ((float)tes.find(test_id)->second.second / 100))));
	}
	marks.close();
	// Will append student_id, course_id, and averageCourse grade to vector
	std::vector<std::tuple<int, int, float>> csv_to_json;
	float sumAvg = 0.00;
	int cid = tes.find(std::get<1>(coursesStudentsTook[0]))->second.first;
	int sid = std::get<0>(coursesStudentsTook[0]);
	for (int i = 0; i < coursesStudentsTook.size(); i++)
	{
		// If current course id isn't equal to index course id stop adding and push to csv_to_json vector
		if (cid != tes.find(std::get<1>(coursesStudentsTook[i]))->second.first || sid != std::get<0>(coursesStudentsTook[i]))
		{
			csv_to_json.push_back(std::make_tuple(sid, cid, sumAvg));
			cid = tes.find(std::get<1>(coursesStudentsTook[i]))->second.first;
			sid = std::get<0>(coursesStudentsTook[i]);
			sumAvg = 0.00;
			sumAvg += std::get<2>(coursesStudentsTook[i]);
		}
		else
		{
			sumAvg += std::get<2>(coursesStudentsTook[i]);
		}
		// Was having an issue where all courses were being appended but the last one
		// this guards against that issue as it will push anything that is left before
		// loop terminates
		if (i == coursesStudentsTook.size() - 1)
		{
			csv_to_json.push_back(std::make_tuple(sid, cid, sumAvg));
		}
	}

	// Will put student id and average of courses taken into map
	std::map<int, float> totalAverage;
	float totalAvg = 0.00;
	int coursesCount = 0;
	sid = std::get<0>(csv_to_json[0]);
	for (int i = 0; i < csv_to_json.size(); i++)
	{
		if (sid == std::get<0>(csv_to_json[i]))
		{
			totalAvg += std::get<2>(csv_to_json[i]);
			coursesCount++;
		}
		else
		{
			totalAvg /= coursesCount;
			totalAverage.insert(std::pair<int, float>(sid, totalAvg));
			totalAvg = 0.00;
			coursesCount = 0;
		}
	}

	// JSON OUTPUT
	std::ofstream output(argv[5]);
	output << "{\n";
	output << "  \"students\": [\n";
	cid = std::get<1>(csv_to_json[0]);
	sid = std::get<0>(csv_to_json[0]);
	for (int i = 0; i < csv_to_json.size(); i++)
	{
		// Will print student
		if (sid == std::get<0>(csv_to_json[i]) && i == 0)
		{
			output << "    {\n";
			output << "      \"id\": " + std::to_string(sid);
			output << ",\n";
			output << "      \"name\": \"" + stu.find(sid)->second + "\",\n";
			output << "      \"totalAverage\": " + std::to_string(totalAverage.find(sid)->second) + ",\n";
			output << "      \"courses\": [\n";
		}
		if (sid != std::get<0>(csv_to_json[i]) && i != 0)
		{
			std::cout << "here" << std::endl;
			sid = std::get<0>(csv_to_json[i]);
			output << "    {\n";
			output << "      \"id\": " + std::to_string(sid);
			output << ",\n";
			output << "      \"name\": \"" + stu.find(sid)->second + "\",\n";
			output << "      \"totalAverage\": " + std::to_string(totalAverage.find(sid)->second) + ",\n";
			output << "      \"courses\": [\n";
		}
		// Will print course student took with comma
		if (sid == std::get<0>(csv_to_json[i]) && sid == std::get<0>(csv_to_json[i + 1]))
		{
			cid = std::get<1>(csv_to_json[i]);
			output << "        {\n";
			output << "          \"id\": " + std::to_string(cid) + ",\n";
			output << "          \"name\": \"" + cou.find(cid)->second.first + "\",\n";
			output << "          \"teacher\": \"" + cou.find(cid)->second.second + "\",\n";
			output << "          \"courseAverage\": " + std::to_string(std::get<2>(csv_to_json[i])) + ",\n";
			output << "        },\n";
		}
		// Will print final course student took without trailling comma
		if (sid == std::get<0>(csv_to_json[i]) && sid != std::get<0>(csv_to_json[i + 1]))
		{
			cid = std::get<1>(csv_to_json[i]);
			output << "        {\n";
			output << "          \"id\": " + std::to_string(cid) + ",\n";
			output << "          \"name\": \"" + cou.find(cid)->second.first + "\",\n";
			output << "          \"teacher\": \"" + cou.find(cid)->second.second + "\",\n";
			output << "          \"courseAverage\": " + std::to_string(std::get<2>(csv_to_json[i])) + ",\n";
			output << "        }\n";
			output << "      ]\n";
			output << "    },\n";
			sid = std::get<0>(csv_to_json[i + 1]);
		}
		// std::cout << std::get<0>(csv_to_json[i]) << " " << std::get<1>(csv_to_json[i]) << " " << std::get<2>(csv_to_json[i]) << std::endl;
	}
	output << "    \n  ]\n";
	output << "}";
	output.close();
}
