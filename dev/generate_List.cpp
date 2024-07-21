//cpp file to generate list for the tlb analysis
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>


struct Node {
    int data;
    Node* next;
};


//used to store the nodes distributed in memory
//otherwise the nodes would be too close too each 
//which would lead to every TLB having >99% hit rate
Node* createDistributedList(int n, int extraNodes) {
    int totalNodes = n + extraNodes;
    std::vector<Node*> pool(totalNodes);

    
    for (int i = 0; i < totalNodes; ++i) {
        pool[i] = new Node;
        pool[i]->data = -1;  
        pool[i]->next = nullptr;
    }

    
    std::random_device generator;
    std::shuffle(pool.begin(), pool.end(), generator);

    
    Node* head = pool[0];
    head->data = 0;
    Node* current = head;

    for (int i = 1; i < n; ++i) {
        current->next = pool[i];
        current = current->next;
        current->data = i;
    }

    
    for (int i = n; i < totalNodes; ++i) {
        delete pool[i];
    }

    return head;
}

int listSum(Node* head) {
    int sum = 0;
    Node* current = head;

    while (current) {
        sum += current->data;
        current = current->next;
    }

    return sum;
}


// Used to generate the csv file to simulate the memory access,
// when iterating through the linked list
// The write requests simulate the creation of the linked list.
// For the evaluation of the sum in the README.md file the files,
// which consist only of read requests were taken
// The read requests simulate the iteration through the linked list
// to sum up its values
void logMemoryWrite(Node* head, std::ofstream &file){
    Node* current = head;
    while (current) {
        uintptr_t value = reinterpret_cast<uintptr_t>(current);
        uint32_t low = value & 0xFFFFFFFF;

        uintptr_t nextValue = reinterpret_cast<uintptr_t>(current->next);
        uint32_t nextLow = nextValue & 0xFFFFFFFF;

        file << "w" << "," << low << "," << current->data << std::endl;
        file << "w" << "," << low + 16 << "," << nextLow << std::endl;

        current = current->next;
    }
}

void logMemoryRead(Node* head, std::ofstream &file){
    Node* current = head;
    while (current) {
        uintptr_t value = reinterpret_cast<uintptr_t>(current);
        uint32_t low = value & 0xFFFFFFFF;


        file << "r" << "," << low << "," << std::endl;
        file << "r" << "," << low + 16 << "," << std::endl;

        current = current->next;
    }
}

void freeMemory(Node* head) {
    Node* current = head;
    while (current) {
        Node* next = current->next;
        delete current;
        current = next;
    }
}

int main () {
    Node* head = createDistributedList(50000, 10000);
    int sum = listSum(head);

    std::string filename = "requests.csv";
    std::ofstream file(filename);

    logMemoryWrite(head, file);
    logMemoryRead(head, file);

    std::cout << "Sum: " << sum << std::endl;

    freeMemory(head);
    return 0;
}
