#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <unordered_set>


using namespace std;
class solution {

private:
bool DEBUG;
int clck;
vector<string> vect_lines;
vector<int>* t_vars;
vector <string> instructionsVector;
vector <int > solutionVector;
vector <string> operators;
//int operatorCounter;
int instructionCounter = 1;
vector <int> registerPositions;
string currentInstruction;
string currentOperator;
int programCounter = 0;
bool isBranchTaken = false;
bool end = false;




public :

solution(ifstream &file_in,int clck_in = 10 ,bool DEBUG_in = false);
void dbg(const string &msg);
vector<int>* alu();
int mips_clock();
void setInputRegisters(string inputGivenByFile);
void extractOperators();
void printRegisters (vector <string> inputVectors);
void executeMipsInstruction();
void initializeUnorderedSet();
void extractArithmaticOperands();
void printIntegerRegisters (vector <int>* inputVectors);
void executeArithmaticOperation();
void executeArithmaticImmediateOperation();
void extractArithmaticImmediateOperands();
void stringSplit(string s, string delimiter, vector <string> &splitResultVector);
bool stringStartWith(string s, string delimiter);
void extractUnconditionalBranch(string &jumpLabel);
void extractConditionalBranch(string &jumpLabel);
void getNextInstruction();
int findBranchLabelPosition(string branch);
void splitInstruction(string s, string delimiter,vector <string> &splitVector);

};
//

vector<int>* solution::alu(){
	while(this->clck){
		if(!mips_clock()) {
			continue;
		} else {
			string inputGivenByFile = this->instructionsVector.at(0);
			this->setInputRegisters(inputGivenByFile);
			this->instructionsVector.erase(this->instructionsVector.begin());
			this->extractOperators();
			executeMipsInstruction();
			return this->t_vars;

		}

	}
}


solution::solution(ifstream &file_in,int clck_in,bool DEBUG_in){
	this->t_vars = new vector <int>;
	this->clck = clck_in;
	this->DEBUG = DEBUG_in;
	string inputline;
	while (getline(file_in, inputline)) {
		this->instructionsVector.push_back(inputline);
	}
}



int solution::mips_clock() {
chrono::milliseconds timespan(clck); 

this_thread::sleep_for(timespan);
static int cycle = 0;
if (cycle == 0 )
	cycle = 1;
else 
	cycle = 0;
return cycle;
}

void solution::setInputRegisters(string inputGivenByFile ){
	stringstream ss(inputGivenByFile);
	int i;
	while (ss >> i) {
		this->t_vars->push_back(i);
		if (ss.peek() == ',')
			ss.ignore();
	}
	if(false){
	for(int i =0; i <this->t_vars->size();i++){
				cout << this->t_vars->at(i) <<endl;
			}
	}
}

void solution::extractOperators(){
	int i = 0;
	string compareString;
	do{
		stringstream ss (this->instructionsVector.at(i));
		string str;
		ss >> str;
		this->operators.push_back(str);
		i++;
	}while (this->instructionsVector.at(i).compare("end") != 0);
	this->operators.push_back("end");

}

void solution::printIntegerRegisters (vector <int>* inputVectors){
	for(int i =0; i <t_vars->size();i++){
		cout << "R("<< i << ")"<< "::" << t_vars->at(i) << "  ";
	}
}

void solution::executeMipsInstruction(){
	int i =0;
	this->getNextInstruction();
	while(!this->end) {
		this->isBranchTaken = false;
		cout << "Current Instruction :: " << this->currentInstruction << endl;
		cout << "Registers State " << endl;
		this->printIntegerRegisters(this->t_vars);
		cout << endl;
	if((this->currentOperator.compare("add") == 0) || (this->currentOperator.compare("sub") == 0) ||
			(this->currentOperator.compare("mul") == 0) || (this->currentOperator.compare("div") == 0)){
		this->extractArithmaticOperands();
		this->executeArithmaticOperation();
	}
	if((this->currentOperator.compare("addi") == 0) || (this->currentOperator.compare("subi") == 0) ||
				(this->currentOperator.compare("muli") == 0) || (this->currentOperator.compare("divi") == 0)){
		this->extractArithmaticImmediateOperands();
		this->executeArithmaticImmediateOperation();
	}

	if((this->currentOperator.compare("bnq") == 0) || (this->currentOperator.compare("beq") == 0)){
			string jumpLabel ;
			this->extractConditionalBranch(jumpLabel);
			if(this->currentOperator.compare("bnq") == 0){
					if(this->t_vars->at(this->registerPositions.at(0)) != this->t_vars->at(registerPositions.at(1))){
						int index = this->findBranchLabelPosition(jumpLabel);
						this->programCounter = index;
						this->isBranchTaken = true;

					}
		}
			else{
				if(this->t_vars->at(this->registerPositions.at(0)) == this->t_vars->at(registerPositions.at(1))){
					int index = this->findBranchLabelPosition(jumpLabel);
					this->programCounter = index;
					this->isBranchTaken = true;

			}
	}
	}

	if((this->currentOperator.compare("b") == 0)){
		string jumpLabel;
		this->extractUnconditionalBranch(jumpLabel);
		int index = this->findBranchLabelPosition(jumpLabel);
		this->isBranchTaken = true;
		this->programCounter = index;
		}
	if(!this->isBranchTaken){
		this->programCounter++;
}
	cout << endl;
	this->getNextInstruction();
}
}

void solution::getNextInstruction(){
	string delimiter = " ";

		vector <string> statementVector;
		this->currentInstruction = this->instructionsVector.at(this->programCounter);
		this->splitInstruction(this->currentInstruction, delimiter, statementVector);
		if(statementVector.size() == 3 && statementVector.at(1).compare("beq") !=0 && statementVector.at(1).compare("bnq") !=0){
			this->currentOperator = statementVector.at(1);
			this->currentInstruction = statementVector[1] + " " + statementVector[2];
		}else{
		this->currentOperator = statementVector.at(0);
		if(this->currentInstruction.compare("end") == 0){
			this->end = true;
			}
		}
}

int solution::findBranchLabelPosition(string jumpLabel){
	int index;
	for (int i=0;i<this->operators.size();i++){
		if(this->operators.at(i).compare(jumpLabel) == 0){
			return i;
		}
	}
}

void solution::extractUnconditionalBranch(string &jumpLabel){
	stringstream ss (this->currentInstruction);
	string operators;
	while (ss >> operators){
		if(ss.peek() == ' '){
			ss.ignore();

	}
	}
	jumpLabel = operators;
}

void solution::extractConditionalBranch(string &jumpLabel){
	vector <string> conditionalBranch;
	conditionalBranch.clear();
	registerPositions.clear();
	string s = this->currentInstruction;
	size_t pos = 0;
	std::string token;
	string delimiter = " ";
	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		s.erase(0, pos + delimiter.length());
	}
	delimiter = ",";
	while ((pos = s.find(delimiter)) != std::string::npos) {
	token = s.substr(0, pos);
		conditionalBranch.push_back(token);
		s.erase(0, pos + delimiter.length());
	}
	jumpLabel = s;
	for(int i=0;i < conditionalBranch.size() ;i++){
		string s = conditionalBranch.at(i);
			token = conditionalBranch.at(i).substr(1, s.size());
			registerPositions.push_back(stoi(token,nullptr));
		}
	}


void solution :: extractArithmaticOperands(){
	stringstream ss (this->currentInstruction);
	registerPositions.clear();
	string operators;
	while (ss >> operators){
		if(ss.peek() == ' '){
			ss.ignore();
	}

	for (int i =0 ; i< operators.size(); i++){
		if(operators[i] == '$'){
				string s = operators.substr(i+1,1);
				registerPositions.push_back(stoi(s,nullptr));
			}

		}
	}
}

void solution :: extractArithmaticImmediateOperands(){
	string arithmaticString = this->currentInstruction;
	registerPositions.clear();
	vector <string> splitResultVector;
	string delimiter = " ";
	string token;
	stringSplit(arithmaticString,  delimiter, splitResultVector);
	//addi $2,$3,5
	string resisterPosition = splitResultVector.at(1);
	delimiter = ",";
	splitResultVector.clear();
	stringSplit(resisterPosition,  delimiter, splitResultVector);
	for(int i=0;i < splitResultVector.size() ;i++){
		string s = splitResultVector.at(i);
		string delimiter = "$";
		bool result = stringStartWith(s,delimiter);
		if(result){
			registerPositions.push_back(stoi((s.substr(1,s.size())),nullptr));
		}else{
			registerPositions.push_back(stoi((s),nullptr));
		}
		}
}


void solution::stringSplit(string s, string delimiter, vector <string> &splitResultVector){
	splitResultVector.clear();
	int pos = 0;
	string token;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		splitResultVector.push_back(token);
		s.erase(0, pos + delimiter.length());
	}
	splitResultVector.push_back(s);
}

void solution::splitInstruction(string s, string delimiter, vector <string> &stringSplit){
	int pos = 0;
	string token;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		stringSplit.push_back(token);
		s.erase(0, pos + delimiter.length());
	}
	stringSplit.push_back(s);
}

bool solution::stringStartWith(string str, string delimiter){

		for(int i=0;i < str.size() ;i++){
			string token = str.substr(0, 1);
			if(token.compare(delimiter) == 0){
				return true;
			}
			else
				return false;
		}
}




void solution::executeArithmaticOperation(){
	if(this->currentOperator.compare("add") == 0){
		t_vars->at(registerPositions.at(0)) = t_vars->at(registerPositions.at(1)) + t_vars->at(registerPositions.at(2));
	}
	if(this->currentOperator.compare("sub") == 0){
		t_vars->at(registerPositions.at(0)) = t_vars->at(registerPositions.at(1)) - t_vars->at(registerPositions.at(2));

	}
	if(this->currentOperator.compare("mul") == 0){
		t_vars->at(registerPositions.at(0)) = t_vars->at(registerPositions.at(1)) * t_vars->at(registerPositions.at(2));
	}
	if(this->currentOperator.compare("div") == 0){
		t_vars->at(registerPositions.at(0)) = t_vars->at(registerPositions.at(1)) / t_vars->at(registerPositions.at(2));
	}
//	this->printIntegerRegisters(t_vars);

}
void solution::executeArithmaticImmediateOperation(){
	if(this->currentOperator.compare("addi") == 0){
		t_vars->at(registerPositions.at(0)) = t_vars->at(registerPositions.at(1)) + registerPositions.at(2);
	}
	if(this->currentOperator.compare("subi") == 0){
		t_vars->at(registerPositions.at(0)) = t_vars->at(registerPositions.at(1)) - registerPositions.at(2);
	}
	if(this->currentOperator.compare("muli") == 0 ){
		t_vars->at(registerPositions.at(0)) = t_vars->at(registerPositions.at(1)) * registerPositions.at(2);
	}
	if(this->currentOperator.compare("divi") == 0){
		t_vars->at(registerPositions.at(0)) = t_vars->at(registerPositions.at(1)) / registerPositions.at(2);
	}
//	this->printIntegerRegisters(inputRegisters);

}
