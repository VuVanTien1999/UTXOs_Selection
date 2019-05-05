//#include "gurobi_c++.h"
//#include <iostream>
//#include <vector>
//#include <string>
//
//// read file
//#include <fstream>
//#include <iostream>
//#include <filesystem>
//#include <cstdlib>
//#include <fstream>
//
//using namespace std;
//
//// begin add model
//
//long numOfUTXOs, maxBlockSize, i;
//long numOfVout;
//bool isFeasible = true;
//
//try {
//	// Create an environment
//	GRBEnv env = GRBEnv(true);
//	env.set("LogFile", "test1.log");
//	env.start();
//
//	// Create an empty model
//	GRBModel model = GRBModel(env);
//
//	vector<long> size_in_vector(numOfUTXOs);
//	vector<long> size_out_vector(numOfVout);
//	vector<long> value_UTXOs_vector(numOfUTXOs);
//	vector<long> value_out_vector(numOfUTXOs);
//	// TODO: assign value from file to each element
//	unsigned long a; // TODO: assign sum of output that send to receivers
//	double m, n;	// lower bound and upper bound - %
//					// TODO: assign value to m, n
//
//
//	// Add constraint (2)
//	long T;	// TODO: assign value to T --- Threshold
//	for (i = 0; i < numOfVout; i++) {
//		if (value_out_vector[i] >= T)
//			continue;
//		else {
//			isFeasible = false;
//			cout << "Can't optimize: output value is smaller than dust threshold!!!" << endl;
//			return -1;
//		}
//	}
//
//	cout << "m*a >= T -> T/a = " << (double)T / a << endl;
//
//	// Create variables
//	GRBVar z_v = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_INTEGER, "z_v");	// z_value
//	GRBVar z_s = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_INTEGER, "z_s");	// z_size
//	GRBVar z_b = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "z_b");			// z_binary
//
//	vector<GRBVar> c_vector(numOfUTXOs);	// binary variable for choosing special UTXO
//	for (i = 0; i < numOfUTXOs; i++) {
//		c_vector[i] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, ("c_element_" + to_string(i)));
//	}
//
//	vector<GRBVar> x_vector(numOfUTXOs);
//	for (i = 0; i < numOfUTXOs; i++) {
//		x_vector[i] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, ("x_element_" + to_string(i)));
//	}
//
//	// -------------Phase 1-------------------
//	// Add constraint (4) c_i: binary variable
//	for (i = 0; i < numOfUTXOs; i++) {
//		model.addConstr(c_vector[i] * m*a <= c_vector[i] * value_UTXOs_vector[i], "c4_1_" + to_string(i));
//		model.addConstr(c_vector[i] * value_UTXOs_vector[i] <= c_vector[i] * n*a, "c4_2_" + to_string(i));
//	}
//
//	GRBLinExpr numOfCi = 0.0;
//	for (i = 0; i < numOfUTXOs; i++) {
//		numOfCi += c_vector[i];
//	}
//	model.setObjective(numOfCi, GRB_MAXIMIZE);	// phase 1
//	model.optimize();
//	// -------------End phase 1-------------------
//
//	// -------------Phase 2-------------------
//	// Add constraint (0)
//	GRBLinExpr y = 0.0;
//	for (i = 0; i < numOfUTXOs; i++) {
//		y = y + size_in_vector[i] * x_vector[i];
//	}
//	for (i = 0; i < numOfUTXOs; i++) {
//		y = y + size_out_vector[i];
//	}
//	y = y + z_s;
//	model.addConstr(y <= maxBlockSize, "c0");
//
//	// Add constraint (1)
//	GRBLinExpr sum_before = 0.0;
//	GRBLinExpr sum_after = 0.0;
//	for (i = 0; i < numOfUTXOs; i++) {
//		sum_before += value_UTXOs_vector[i] * x_vector[i];
//	}
//	for (i = 0; i < numOfVout; i++) {
//		sum_after += value_out_vector[i];
//	}
//	double alpha;	// TODO: assign value to alpha
//	sum_after += alpha * y + z_v;
//
//	model.addConstr(sum_before == sum_after, "c1");
//
//	// Add constraint (3): version_2
//	long beta;	// TODO: assign value to beta
//	long epsilon;	// TODO: assign value to epsilon
//	model.addConstr(0 <= z_v, "c3_0");
//	model.addConstr(z_v <= (z_v - (epsilon - 1))*z_b + (epsilon - 1), "c3_1");
//	model.addConstr(z_v >= epsilon * z_b, "c3_2");
//	model.addConstr(z_s == z_b * beta, "c3_3");
//
//
//	// Add constraint (5): relationship between c_i and x_i
//	for (i = 0; i < numOfUTXOs; i++) {
//		model.addConstr(x_vector[i] >= c_vector[i].get(GRB_DoubleAttr_X), "c5_" + to_string(i));
//		// x_vector[i] >= c_vector[i].get(GRB_DoubleAttr_X)
//	}
//
//	// Set objective
//
//	model.setObjective(y, GRB_MINIMIZE);		// phase 2
//	// Optimize model
//	model.optimize();
//
//	// -------------End phase 2-------------------
//
//}
//catch (GRBException e) {
//	cout << " Error code = " << e.getErrorCode() << endl;
//	cout << e.getMessage() << endl;
//}
//catch (...) {
//	cout << " Exception during optimization " << endl;
//}