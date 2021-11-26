#include <iostream>
#include <fstream>
#include <string>
#include <vector>

// TODO: Combine all CRDT object headers into one header
#include "2P_Set.h"
#include "G_Counter.h"
#include "G_Set.h"
#include "PN_Counter.h"

// TODO: Maybe create one common class that can be used to instantiate all CRDT objects "class CRDT"
std::vector <TwoPhase_Set<int>> twoPhase;
std::vector <g_Counter<int, int>> gCounter;
std::vector <g_Set<int>> gSet;
std::vector <pn_Counter<int, int>> pnCounter;

int main() {

	const std::string inputNew = "N";
	const std::string inputExport = "E";
	const std::string inputImport = "I";
	const std::string update = "U";

	std::string input;
	std::string crdtInput;
	std::string values;

	while (1) {

		std::cout << "CRDT Library - Enter 'N' to create a new CRDT, 'E' to export to CSV, 'I' to import from CSV, 'U' to update an existing CRDT: ";

		std::cin >> input;
		std::cout << std::endl;

		if (input.compare(inputNew) == 0) {

			std::cout << "Enter (0) to create a 2P_Set, (1) to create a G_Set, (2) to create a PN_Counter, (3) to create a G_Counter: ";
			
			std::cin >> input;

			if (input.compare("0") == 0) {
				std::cout << "Creating 2P_Set with object ID " << twoPhase.size() << " and global ID 0" << std::endl;
				TwoPhase_Set<int>* new2PSet = new TwoPhase_Set<int>(twoPhase.size());
				twoPhase.push_back(*new2PSet);
			}
			else if (input.compare("1") == 0) {
				std::cout << "Creating G_Set with object ID " << gSet.size() << " and global ID 1" << std::endl;
				g_Set<int>* newGSet = new g_Set<int>(gSet.size());
				gSet.push_back(*newGSet);
			}
			else if (input.compare("2") == 0) {
				std::cout << "Creating PN_Counter with object ID " << pnCounter.size() << " and global ID 2" << std::endl;
				pn_Counter<int, int>* newPnCounter = new pn_Counter<int, int>(pnCounter.size());
				pnCounter.push_back(*newPnCounter);
			}
			else if (input.compare("3") == 0) {
				std::cout << "Creating G_Counter with object ID " << gCounter.size() << " and global ID 3" << std::endl;
				g_Counter<int, int>* newGCounter = new g_Counter<int, int>(gCounter.size());
				gCounter.push_back(*newGCounter);
			}
		}
		else if (input.compare(update) == 0) {

			std::cout << "Enter the global ID of the CRDT to update (0, 1, 2, or 3):";
			std::cin >> input;

			if (input.compare("0") == 0) {
				std::cout << "Enter the object ID of the CRDT to update: ";
				std::cin >> input;
				unsigned objectID = std::stoi(input);

				if (objectID >= twoPhase.size())
					std::cout << "Invalid ID!" << std::endl;

				else {

					std::cout << "Enter the element to insert: ";
					std::cin >> input;

					twoPhase.at(objectID).add(std::stoi(input));
				}
			}
			else if (input.compare("1") == 0) {
				std::cout << "Enter the object ID of the CRDT to update: ";
				std::cin >> input;
				unsigned objectID = std::stoi(input);

				if (objectID >= twoPhase.size())
					std::cout << "Invalid ID!" << std::endl;

				else {

					std::cout << "Enter the element to insert: ";
					std::cin >> input;

					gSet.at(objectID).add(std::stoi(input));
				}
			}
			else if (input.compare("2") == 0) {
				std::cout << "Enter the object ID of the CRDT to update: ";
				std::cin >> input;
				unsigned objectID = std::stoi(input);

				if (objectID >= twoPhase.size())
					std::cout << "Invalid ID!" << std::endl;

				else {

					std::cout << "Enter the value to increment by (negative number for decrement): ";
					std::cin >> input;

					pnCounter.at(objectID).add(std::stoi(input));
				}
			}
			else if (input.compare("3") == 0) {
				std::cout << "Enter the object ID of the CRDT to update: ";
				std::cin >> input;
				unsigned objectID = std::stoi(input);

				if (objectID >= twoPhase.size())
					std::cout << "Invalid ID!" << std::endl;

				else {

					std::cout << "Enter the value to increment by: ";
					std::cin >> input;

					gCounter.at(objectID).add(std::stoi(input));
				}
			}
		}
		else if (input.compare(inputExport) == 0) {

			std::ofstream newFile("crdt.csv");
			newFile << "Global ID,";
			newFile << "Object Type,";
			newFile << "Object ID,";
			newFile << "Number of Elements,";
			newFile << "Data\n";

			for (int i = 0; i < 4; i++) {

				if (i == 0) {

					for (unsigned j = 0; j < twoPhase.size(); j++) {

						newFile << i << ",";
						newFile << "2P Set,";
						newFile << twoPhase[j].get_id() << ",";
						newFile << twoPhase[j].set.size() << ",";
						int count = 0;

						for (auto k = twoPhase[j].set.begin(); k != twoPhase[j].set.end(); k++) {

							count++;
							if (count == twoPhase[j].set.size())
								newFile << *k;
							else
								newFile << *k << ",";

						}

						newFile << "\n";
					}

				}
				else if (i == 1) {

					for (unsigned j = 0; j < gSet.size(); j++) {

						newFile << i << ",";
						newFile << "GSet,";
						newFile << gSet[j].get_id() << ",";
						newFile << gSet[j].set.size() << ",";
						int count = 0;

						for (auto k = gSet[j].set.begin(); k != gSet[j].set.end(); k++) {

							count++;
							if (count == gSet[j].set.size())
								newFile << *k;
							else
								newFile << *k << ",";

						}

						newFile << "\n";

					}

				}
				else if (i == 2) {

					for (unsigned j = 0; j < pnCounter.size(); j++) {

						newFile << i << ",";
						newFile << "PN Counter,";
						newFile << pnCounter[j].get_id() << ",";
						newFile << "1" << ",";
						newFile << pnCounter[j].get_total_val() << "\n";
					}
				}

				else if (i == 3) {

					for (unsigned j = 0; j < gCounter.size(); j++) {

						newFile << i << ",";
						newFile << "GCounter,";
						newFile << gCounter[j].get_id() << ",";
						newFile << "1" << ",";
						newFile << gCounter[j].get_total_val() << "\n";

					}

				}
			}

		}
		else if (input.compare(inputImport) == 0) {

			// TODO: Importing from CSV

		}

	}
}
