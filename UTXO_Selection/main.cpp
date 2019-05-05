#include "gurobi_c++.h"
#include <iostream>
#include <vector>
#include <string>
#include <numeric>

// read file
#include <fstream>
#include <iostream>
#include <filesystem>
#include <cstdlib>
#include <fstream>
#include <time.h>

using namespace std;
namespace fs = std::experimental::filesystem;

struct Vin {
	int id;
	long size;
	long long value;
	int confirm;
	int vout;
	int chosen;
	string txid;
};

struct Vout {
	int id;
	long size;
	long long value;
};

struct record {
	string name_of_file;
	int number_of_UTXOs;
	int number_of_output;
	long long outValue;
	int max_block_size;
	double fee_rate;
	int dust_threshold;
	int epsilon;
	int beta;
	int txsize;
	int iosize;
	bool change_output;
	long long change_output_value;
	vector<Vin> input;
	vector<Vout> output;
};


int main(int argc, char * argv[])
{
	clock_t begin = clock();

	// read file
	string x;
	ifstream inFile;
	string path = "C:/Users/Vu Van Tien/Desktop/dataset/dataset0_copy/data_txt";
	vector<record> instance;
	for (const auto & entry : fs::directory_iterator(path)) {
		inFile.open(entry.path());
		if (!inFile) {
			cout << "Unable to open file" << entry.path();
			system("pause");
			continue;
		}
		cout << entry.path() << endl;	// check error
		string temp_path = entry.path().u8string();
		//cout << "this is string: " << temp_path << endl;
		temp_path.erase(0, path.length()+1);
		//cout << "after remove substring: " << temp_path << endl;
		temp_path.erase(temp_path.find_first_of("."), 4);
		//cout << "after remove substring: " << temp_path << endl;
		//system("pause");				// check error
		record temp_rec;
		temp_rec.name_of_file = temp_path;
		while (inFile >> x) {
			// cout << x << endl;
			if (x == "coutValue") {
				inFile >> temp_rec.number_of_UTXOs >> temp_rec.number_of_output >> temp_rec.outValue >> temp_rec.max_block_size
					>> temp_rec.fee_rate >> temp_rec.dust_threshold >> temp_rec.epsilon >> temp_rec.beta >> temp_rec.txsize
					>> temp_rec.iosize >> temp_rec.change_output >> temp_rec.change_output_value;
			}
			if (x == "txid") {
				Vin temp_input;
				string temp;
				for (int i = 0; i < temp_rec.number_of_UTXOs; i++) {
					inFile >> temp_input.id >> temp_input.size >> temp_input.value >> temp_input.confirm >> temp_input.vout
						>> temp >> temp;
					temp_rec.input.push_back(temp_input);
				}
				for (int i = 0; i < 8; i++) { inFile >> temp; }
				Vout temp_output;
				for (int i = 0; i < temp_rec.number_of_output; i++) {
					inFile >> temp_output.id >> temp_output.size >> temp_output.value;
					temp_rec.output.push_back(temp_output);
				}
			}
		}
		instance.push_back(temp_rec);
		inFile.close();

	}
	// end read file
	// cout << "read ok" << endl;

	ofstream output;
	output.open("my_model.csv");
	if (output.is_open()) {
		cout << "File opened successfully!" << endl;
		//system("pause");
	}
	else {
		cout << "Fail to open file" << endl;
		system("pause");
	}
	// first row
	output << "FileName, NoSelected, TxSize" << endl;

	for (int k = 0; k < instance.size(); k++) { // k < instance.size()	// check error
		cout << "-----|||  Instance: " << k << endl;
		cout << "num_of_UTXOs: " << instance[k].number_of_UTXOs
			<< "-----alpha: " << instance[k].fee_rate << endl;

		// begin model

		long numOfUTXOs, maxBlockSize, i;
		long numOfVout;
		bool isFeasible = true;

		// TODO: assign value to numOfUTXOs, maxBlockSize
		numOfUTXOs = instance[k].number_of_UTXOs;
		maxBlockSize = instance[k].max_block_size;
		numOfVout = instance[k].number_of_output;

		try {
			// Create an environment
			GRBEnv env = GRBEnv(true);
			env.set("LogFile", "test1.log");
			env.start();

			// Create an empty model
			GRBModel model = GRBModel(env);

			vector<long> size_in_vector;
			for (i = 0; i < numOfUTXOs; i++) {
				cout << instance[k].number_of_UTXOs << endl;
				cout << "still ok" << endl;
				cout << instance[k].input[i].id << endl;
				size_in_vector.push_back(instance[k].input[i].size);
			}
			vector<long> size_out_vector;
			for (i = 0; i < numOfVout; i++) {
				size_out_vector.push_back(instance[k].output[i].size);
			}
			vector<long> value_UTXOs_vector;
			for (i = 0; i < numOfUTXOs; i++) {
				value_UTXOs_vector.push_back(instance[k].input[i].value);
				cout << "-----" << instance[k].input[i].value << endl;	// check error
			}
			// check error 2L
			// cout << "---------------" << value_UTXOs_vector[0] << endl;
			// cout << "----------------" << numOfUTXOs << endl;
			vector<long> value_out_vector;
			for (i = 0; i < numOfVout; i++) {
				value_out_vector.push_back(instance[k].output[i].value);
			}
			// TODO: assign value from file to each element
			unsigned long a; // TODO: assign sum of output that send to receivers
			a = accumulate(value_out_vector.begin(), value_out_vector.end(), 0);
			long T;	// TODO: assign value to T --- Threshold
			T = instance[k].dust_threshold;
			double m, n;	// lower bound and upper bound - %
							// TODO: assign value to m, n
			//m = floor((double)T / a * 100 + 5) / 100.0;		// plus 5%
				//cout << "--------------|||  m*a >= T -> T/a = " << (double)T / a << endl;
				// *100 lam tron len roi chia 100
			m = 0.05; // for specified value
			n = 0.66;
			cout << k << "__(lower bound) m = " << m << " (upper bound) n = " << n << endl;

			// Add constraint (2)
			for (i = 0; i < numOfVout; i++) {
				if (value_out_vector[i] >= T)
					continue;
				else {
					isFeasible = false;
					//cout << "Can't optimize: output value is smaller than dust threshold!!!" << endl;
					//system("pause");
					//return -1;
					break;
				}
			}
			if (!isFeasible) {
				cout << "Can't optimize: output value is smaller than dust threshold!!!" << endl;
				cout << "-------> File name: " << instance[k].name_of_file << endl;
				system("pause");
				continue;	// check error
			}

			// Create variables
			GRBVar z_v = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, "z_v");	// z_value // continuous??
			GRBVar z_s = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_INTEGER, "z_s");	// z_size 
			GRBVar z_b = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "z_b");			// z_binary

			vector<GRBVar> c_vector(numOfUTXOs);	// binary variable for choosing special UTXO
			for (i = 0; i < numOfUTXOs; i++) {
				c_vector[i] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, ("c_element_" + to_string(i)));
			}

			vector<GRBVar> x_vector(numOfUTXOs);
			for (i = 0; i < numOfUTXOs; i++) {
				x_vector[i] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, ("x_element_" + to_string(i)));
			}

			// -------------Phase 1-------------------
			// Add constraint (4) c_i: binary variable
			long m_a = floor(m*a);
			long n_a = floor(n*a);
			cout << m_a << " " << value_UTXOs_vector[0] << endl;
			cout << n_a << " " << value_UTXOs_vector[0] << endl;
			for (i = 0; i < numOfUTXOs; i++) {
				model.addConstr(c_vector[i] * m_a <= c_vector[i] * value_UTXOs_vector[i], "c4_1_" + to_string(i));
				model.addConstr(c_vector[i] * value_UTXOs_vector[i] <= c_vector[i] * n_a, "c4_2_" + to_string(i));
				//model.addConstr(c_vector[i] * m_a <= value_UTXOs_vector[i], "c4_1_" + to_string(i));
				//model.addConstr(c_vector[i] * value_UTXOs_vector[i] <= n_a, "c4_2_" + to_string(i));
			}

			GRBLinExpr numOfCi = 0.0;
			for (i = 0; i < numOfUTXOs; i++) {
				numOfCi += c_vector[i];
			}
			model.setObjective(numOfCi, GRB_MAXIMIZE);	// phase 1
			model.optimize();

			// result
			cout << "--------------------------" << endl;
			cout << "--+++Phase 1: numOfCi: " << model.get(GRB_DoubleAttr_ObjVal) << endl;
			cout << "Value of c_vector: " << endl;
			for (i = 0; i < numOfUTXOs; i++) {
				cout << c_vector[i].get(GRB_StringAttr_VarName) << ": " << c_vector[i].get(GRB_DoubleAttr_X) << endl;
			}
			// cout << a << endl;	// check error
			cout << "--------------------------" << endl;
			
			// -------------End phase 1-------------------

			// -------------Phase 2-------------------
			// Add constraint (0)
			GRBLinExpr y = 0.0;
			for (i = 0; i < numOfUTXOs; i++) {
				y = y + size_in_vector[i] * x_vector[i];
			}
			for (i = 0; i < numOfVout; i++) {
				y = y + size_out_vector[i];
			}
			y = y + z_s;
			model.addConstr(y <= maxBlockSize, "c0");

			// Add constraint (1)
			GRBLinExpr sum_before = 0.0;
			GRBLinExpr sum_after = 0.0;
			for (i = 0; i < numOfUTXOs; i++) {
				sum_before += value_UTXOs_vector[i] * x_vector[i];
			}
			for (i = 0; i < numOfVout; i++) {
				sum_after += value_out_vector[i];
			}
			double alpha;	// TODO: assign value to alpha
			alpha = instance[k].fee_rate;
			sum_after += (alpha * y) + z_v;

			model.addConstr(sum_before == sum_after, "c1"); // check error: ==

			// Add constraint (3): version_2
			unsigned long long sum_of_utxo_value; // TODO: assign sum of output that send to receivers
			sum_of_utxo_value = accumulate(value_UTXOs_vector.begin(), value_UTXOs_vector.end(), 0);
			unsigned long long max_z_v = sum_of_utxo_value - a;
			//cout << "max_z_v: " << max_z_v << endl;	// check error

			long beta;	// TODO: assign value to beta
			beta = instance[k].beta;
			long epsilon;	// TODO: assign value to epsilon
			epsilon = instance[k].epsilon;
			GRBVar z_v_compl = model.addVar(0.00001, GRB_INFINITY, 0.0, GRB_CONTINUOUS, "z_v_complementation");
			//model.addConstr(0 <= z_v, "c3_0");
			//model.addQConstr(z_v <= (z_v - (epsilon - 1))*z_b + (epsilon - 1), "c3_1");

			model.addConstr(z_v + z_v_compl <= (max_z_v - (epsilon))*z_b + (epsilon), "c3_1");
			//model.addConstr(0 <= z_v_compl, "c3_2");
			model.addConstr(epsilon * z_b <= z_v, "c3_3");
			model.addConstr(z_s == z_b * beta, "c3_4");


			// Add constraint (5): relationship between c_i and x_i
			for (i = 0; i < numOfUTXOs; i++) {
				model.addConstr(x_vector[i] >= c_vector[i].get(GRB_DoubleAttr_X), "c5_" + to_string(i));
				// x_vector[i] >= c_vector[i].get(GRB_DoubleAttr_X)
			}

			// Set objective
			model.setObjective(y, GRB_MINIMIZE);		// phase 2
			// Optimize model
			model.optimize();
			
			// result
			cout << "--+++Phase 2" << endl;
			cout << "txsize: " << model.get(GRB_DoubleAttr_ObjVal) << endl;
			cout << "numOfCi: " << numOfCi << endl;
			for (i = 0; i < numOfUTXOs; i++) {
				cout << x_vector[i].get(GRB_StringAttr_VarName) << ": " << x_vector[i].get(GRB_DoubleAttr_X) << endl;
			}
			double numOfXi = 0.0;
			for (i = 0; i < numOfUTXOs; i++) {
				numOfXi += x_vector[i].get(GRB_DoubleAttr_X);
			}
			cout << numOfXi << endl;
			// -------------End phase 2-------------------
			cout << "beta = " << instance[k].beta
				<< "  ||  alpha = " << instance[k].fee_rate
				<< "  ||  epsilon = " << instance[k].epsilon << endl;
			cout << "z_v: " << (floor)(z_v.get(GRB_DoubleAttr_X)) << endl;		// floor
			cout << "z_v_compl: " << z_v_compl.get(GRB_DoubleAttr_X) << endl;
			cout << "z_s: " << z_s.get(GRB_DoubleAttr_X) << endl;
			cout << "z_b: " << z_b.get(GRB_DoubleAttr_X) << endl;
			cout << instance[k].name_of_file << endl;	// check error
			
			// insert row to file
			output << instance[k].name_of_file << ",";
			output << numOfXi << ",";
			output << model.get(GRB_DoubleAttr_ObjVal) << "\n";

			if (instance[k].name_of_file == "5ad47be34c372215dd13d64a")
				system("pause");
		}
		catch (GRBException e) {
			cout << " Error code = " << e.getErrorCode() << endl;
			cout << e.getMessage() << endl;
		}
		catch (...) {
			cout << " Exception during optimization " << endl;
		}
		// end model
		//system("pause");
	}

	output.close();

	clock_t end = clock();
	cout <<  (double)(end - begin) / CLOCKS_PER_SEC << endl;
	

	system("pause");
	return 0;
}