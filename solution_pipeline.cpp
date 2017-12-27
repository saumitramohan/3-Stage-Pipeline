#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <thread>

using namespace std;
class solution {

private:

bool DEBUG;
int clck;
vector<string> vect_lines;
vector<int>* t_vars;		
vector <string> instructionsVector;
//vector <int> inputRegisters;
string fetchedinstruction;
int programCounter = -1;
string instructionForExecution="";
string currentInstruction;
string currentOperator;
bool end = false;
bool elligbleWriteBack = false;
string writeBackOperator;
string writeBackInstruction;
bool isBranchTaken = false;
bool addStall = false;
int findBranchLabelPosition(string jumpLabel);
int clockCycle = -1;
bool reachedEnd = false;


public :

solution(ifstream &file_in,int clck_in = 10 ,bool DEBUG_in = false);
void dbg(const string &msg);
vector<int>* alu();
int mips_clock();
void setInputRegisters(string inputGivenByFile);
void extractOperators();
void executeMipsInstruction();
void fetchDecode();
void printIntegerRegisters (vector <int>* inputVectors);
void executionStageExecute();
vector <string> operators;
void printRegisters (vector <string> inputVectors);
void executeInstruction();
void splitInstruction(string s, string delimiter, vector <string> &stringSplit);
void extractArithmaticOperands();
vector <int> executionRegisterPositions;
vector <int> writeBackRegisterPositions;
void writeBack();
void executeArithmaticOperation();
void extractArithmaticImmediateOperands();
void extractUnconditionalBranch(string &jumpLabel);
void stringSplit(string s, string delimiter, vector <string> &splitResultVector);
bool stringStartWith(string s, string delimiter);
void extractConditionalBranch(string &jumpLabel);
void executeArithmaticImmediateOperation();
};

solution::solution(ifstream &file_in,int clck_in,bool DEBUG_in){

	this->clck = clck_in;
	string inputline;
	this->DEBUG = false;
	this->t_vars = new vector <int>;
	while (getline(file_in, inputline)) {
		this->instructionsVector.push_back(inputline);
	}
	string inputGivenByFile = this->instructionsVector.at(0);
	this->setInputRegisters(inputGivenByFile);
	this->instructionsVector.erase(this->instructionsVector.begin());
	this->extractOperators();



}

void solution::printRegisters (vector <string> inputVectors){
	for(int i =0; i <inputVectors.size();i++){
	}
}

void solution::printIntegerRegisters (vector <int>* inputVectors){
	for(int i =0; i <t_vars->size();i++){
		cout << "R("<< i << ")"<< "::" << t_vars->at(i) << "  ";
	}
}

vector<int>* solution::alu(){
	while(this->clck){
			if(!mips_clock()) {
				continue;
			} else {
				if(this->end){
					cout <<  endl;
					cout << "Clock cycle till end instruction reached execution stage ::" << this->clockCycle <<endl;
					return this->t_vars;
				}
				cout << endl;
				this->clockCycle++;
				cout << "Clock Cycle:" << this->clockCycle << endl;
				executeMipsInstruction();
			}

		}
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

void solution::executeMipsInstruction(){
	this->fetchDecode();
}

void solution::fetchDecode(){
	string delimiter = " ";
	vector <string> operatorVector;
	if(!this->isBranchTaken && !this->end){
		this->programCounter++;
	}
	this->isBranchTaken = false;
	if(!this->addStall){
		this->fetchedinstruction = this->instructionsVector[this->programCounter]; // fetched instruction

	}
	else{
		this->fetchedinstruction = "NO-OP";
		this->addStall = false;
		this->programCounter --;
	}
	stringSplit(this->fetchedinstruction, delimiter, operatorVector);
	if(operatorVector.at(0).compare("beq")==0 || operatorVector.at(0).compare("bnq")==0 || operatorVector.at(0).compare("b")==0){
		this->addStall = true;
	}
	if(this->fetchedinstruction.compare("end") != 0 && !this->reachedEnd){
	cout << "Fetch ::" << this->fetchedinstruction << endl;
	}
	else if(this->fetchedinstruction.compare("end") == 0 && !this->reachedEnd){
		cout << "Fetch ::" << "end" << endl;
		this->reachedEnd = true;
	}
	else{
		this->reachedEnd = true;
	}
	if(!this->end)
	this->executionStageExecute();
	this->instructionForExecution = this->fetchedinstruction;

}

void solution::executionStageExecute(){

	if(this->instructionForExecution ==""){
	}
	else{
		this->executeInstruction();
		cout << "Execution ::" << this->currentInstruction << endl;
		if((this->currentOperator.compare("add") == 0) || (this->currentOperator.compare("sub") == 0) ||
					(this->currentOperator.compare("mul") == 0) || (this->currentOperator.compare("div") == 0)){
			this->extractArithmaticOperands();
		}
		if((this->currentOperator.compare("addi") == 0) || (this->currentOperator.compare("subi") == 0) ||
						(this->currentOperator.compare("muli") == 0) || (this->currentOperator.compare("divi") == 0)){
			this->extractArithmaticImmediateOperands();

		}
		if((this->currentOperator.compare("b") == 0)){
			string jumpLabel;
			this->extractUnconditionalBranch(jumpLabel);
			int index = this->findBranchLabelPosition(jumpLabel);
			this->isBranchTaken = true;
			this->programCounter = index;
		}

		if((this->currentOperator.compare("bnq") == 0) || (this->currentOperator.compare("beq") == 0)){
					string jumpLabel ;
					this->extractConditionalBranch(jumpLabel);
					if(this->currentOperator.compare("bnq") == 0){
							if(this->t_vars->at(this->executionRegisterPositions.at(0)) != this->t_vars->at(executionRegisterPositions.at(1))){
								int index = this->findBranchLabelPosition(jumpLabel);
								this->programCounter = index;
								this->isBranchTaken = true;

							}
				}
					else{
						if(this->t_vars->at(this->executionRegisterPositions.at(0)) == this->t_vars->at(executionRegisterPositions.at(1))){
							int index = this->findBranchLabelPosition(jumpLabel);
							this->programCounter = index;
							this->isBranchTaken = true;

					}
			}
			}
		this->writeBack();
		this->elligbleWriteBack = true;
	}
	this->writeBackOperator = this->currentOperator;
	this->writeBackInstruction = this->currentInstruction;
	this->writeBackRegisterPositions = this->executionRegisterPositions;
	if(this->writeBackOperator.compare("end") == 0){
		this->end = true;
	}
}

void solution::writeBack(){
	if(!elligbleWriteBack){
	}
	else{
		cout << "Write Back ::" << this->writeBackInstruction << endl;
		if((this->writeBackOperator.compare("add") == 0) || (this->writeBackOperator.compare("sub") == 0) ||
							(this->writeBackOperator.compare("mul") == 0) || (this->writeBackOperator.compare("div") == 0)){
			this->executeArithmaticOperation();
		}
		if((this->writeBackOperator.compare("addi") == 0) || (this->writeBackOperator.compare("subi") == 0) ||
								(this->writeBackOperator.compare("muli") == 0) || (this->writeBackOperator.compare("divi") == 0)){
				this->executeArithmaticImmediateOperation();
			}
		cout << "Registers state ::" << endl;
		this->printIntegerRegisters(this->t_vars);
		cout << endl;

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
	printRegisters(this->operators);
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
	executionRegisterPositions.clear();
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
			executionRegisterPositions.push_back(stoi(token,nullptr));
		}
	}


int solution::findBranchLabelPosition(string jumpLabel){
	int index;
	for (int i=0;i<this->operators.size();i++){
		if(this->operators.at(i).compare(jumpLabel) == 0){
			printRegisters(this->operators);
			return i;
		}
	}
}

void solution::executeInstruction(){
		string delimiter = " ";
		vector <string> statementVector;
		this->currentInstruction = this->instructionForExecution;
		this->splitInstruction(this->currentInstruction, delimiter, statementVector);
		if(statementVector.size() == 3 && statementVector.at(1).compare("beq") !=0 && statementVector.at(1).compare("bnq") !=0){
			this->currentOperator = statementVector.at(1);
			this->currentInstruction = statementVector[1] + " " + statementVector[2];
		}else{
		this->currentOperator = statementVector.at(0);
		}
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

void solution :: extractArithmaticOperands(){
	stringstream ss (this->currentInstruction);
	executionRegisterPositions.clear();
	string operators;
	while (ss >> operators){
		if(ss.peek() == ' '){
			ss.ignore();
	}

	for (int i =0 ; i< operators.size(); i++){
		if(operators[i] == '$'){
				string s = operators.substr(i+1,1);
				executionRegisterPositions.push_back(stoi(s,nullptr));
			}

		}
	}
}

void solution::executeArithmaticOperation(){
	if(this->writeBackOperator.compare("add") == 0){
		t_vars->at(writeBackRegisterPositions.at(0)) = t_vars->at(writeBackRegisterPositions.at(1)) + t_vars->at(writeBackRegisterPositions.at(2));
	}
	if(this->writeBackOperator.compare("sub") == 0){
		t_vars->at(writeBackRegisterPositions.at(0)) = t_vars->at(writeBackRegisterPositions.at(1)) - t_vars->at(writeBackRegisterPositions.at(2));

	}
	if(this->writeBackOperator.compare("mul") == 0){
		t_vars->at(writeBackRegisterPositions.at(0)) = t_vars->at(writeBackRegisterPositions.at(1)) * t_vars->at(writeBackRegisterPositions.at(2));
	}
	if(this->writeBackOperator.compare("div") == 0){
		t_vars->at(writeBackRegisterPositions.at(0)) = t_vars->at(writeBackRegisterPositions.at(1)) / t_vars->at(writeBackRegisterPositions.at(2));
	}
	//this->printIntegerRegisters(inputRegisters);

}

void solution::executeArithmaticImmediateOperation(){
	if(this->writeBackOperator.compare("addi") == 0){
		t_vars->at(writeBackRegisterPositions.at(0)) = t_vars->at(writeBackRegisterPositions.at(1)) + writeBackRegisterPositions.at(2);
	}
	if(this->writeBackOperator.compare("subi") == 0){
		t_vars->at(writeBackRegisterPositions.at(0)) = t_vars->at(writeBackRegisterPositions.at(1)) - writeBackRegisterPositions.at(2);
	}
	if(this->writeBackOperator.compare("muli") == 0 ){
		t_vars->at(writeBackRegisterPositions.at(0)) = t_vars->at(writeBackRegisterPositions.at(1)) * writeBackRegisterPositions.at(2);
	}
	if(this->writeBackOperator.compare("divi") == 0){
		t_vars->at(writeBackRegisterPositions.at(0)) = t_vars->at(writeBackRegisterPositions.at(1)) / writeBackRegisterPositions.at(2);
	}

}

void solution :: extractArithmaticImmediateOperands(){
	string arithmaticString = this->currentInstruction;
	executionRegisterPositions.clear();
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
			executionRegisterPositions.push_back(stoi((s.substr(1,s.size())),nullptr));
		}else{
			executionRegisterPositions.push_back(stoi((s),nullptr));
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

//void solution::extractConditionalBranch(string &jumpLabel){
//	vector <string> conditionalBranch;
//	conditionalBranch.clear();
//	executionRegisterPositions.clear();
//	string s = this->currentInstruction;
//	size_t pos = 0;
//	std::string token;
//	string delimiter = " ";
//	while ((pos = s.find(delimiter)) != std::string::npos) {
//		token = s.substr(0, pos);
//		s.erase(0, pos + delimiter.length());
//	}
//	delimiter = ",";
//	while ((pos = s.find(delimiter)) != std::string::npos) {
//	token = s.substr(0, pos);
//		conditionalBranch.push_back(token);
//		s.erase(0, pos + delimiter.length());
//	}
//	jumpLabel = s;
//	for(int i=0;i < conditionalBranch.size() ;i++){
//		string s = conditionalBranch.at(i);
//			token = conditionalBranch.at(i).substr(1, s.size());
//			executionRegisterPositions.push_back(stoi(token,nullptr));
//		}
//	}


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

