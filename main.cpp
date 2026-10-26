#include <iostream>
#include <string>
using namespace std;

const int SIZE = 100;
const int MAX = 100;

struct Parcel {
    int id, priority;
    string customer, destination, status;
};

struct Node {
    Parcel p;
    Node* next;
};

// Hash Map
class HashMap {
    Node* table[SIZE];

public:
    HashMap() {
        for (int i = 0; i < SIZE; i++)
            table[i] = NULL;
    }

    bool insert(Parcel p) {
        int index = p.id % SIZE;
        Node* t = table[index];

        while (t != NULL) {
            if (t->p.id == p.id) return false;
            t = t->next;
        }

        Node* n = new Node{p, table[index]};
        table[index] = n;
        return true;
    }

    Parcel* search(int id) {
        Node* t = table[id % SIZE];

        while (t != NULL) {
            if (t->p.id == id) return &t->p;
            t = t->next;
        }
        return NULL;
    }

    void display() {
        for (int i = 0; i < SIZE; i++) {
            Node* t = table[i];

            while (t != NULL) {
                cout << "\nID: " << t->p.id
                     << "\nCustomer: " << t->p.customer
                     << "\nDestination: " << t->p.destination
                     << "\nPriority: " << t->p.priority
                     << "\nStatus: " << t->p.status << "\n";
                t = t->next;
            }
        }
    }
};

// Queue for normal parcels
class Queue {
    int a[MAX], front, rear;

public:
    Queue() { front = 0; rear = -1; }

    bool empty() { return front > rear; }

    bool add(int id) {
        if (rear == MAX - 1) return false;
        a[++rear] = id;
        return true;
    }

    bool remove(int &id) {
        if (empty()) return false;
        id = a[front++];
        return true;
    }
};

// Priority Queue for urgent parcels
class PriorityQueue {
    int id[MAX], pr[MAX], n;

public:
    PriorityQueue() { n = 0; }

    bool add(int x, int p) {
        if (n == MAX) return false;

        int i = n++;
        while (i > 0 && pr[(i - 1) / 2] < p) {
            id[i] = id[(i - 1) / 2];
            pr[i] = pr[(i - 1) / 2];
            i = (i - 1) / 2;
        }
        id[i] = x;
        pr[i] = p;
        return true;
    }

    bool remove(int &x) {
        if (n == 0) return false;

        x = id[0];
        int lastID = id[--n];
        int lastPR = pr[n];
        int i = 0;

        while (2 * i + 1 < n) {
            int c = 2 * i + 1;
            if (c + 1 < n && pr[c + 1] > pr[c])
                c++;

            if (lastPR >= pr[c]) break;

            id[i] = id[c];
            pr[i] = pr[c];
            i = c;
        }

        if (n > 0) {
            id[i] = lastID;
            pr[i] = lastPR;
        }
        return true;
    }
};

// BST for delivery zones
struct Zone {
    int code;
    string name;
    Zone *left, *right;
};

class BST {
    Zone* root;

    Zone* add(Zone* t, int c, string name) {
        if (t == NULL)
            return new Zone{c, name, NULL, NULL};

        if (c < t->code)
            t->left = add(t->left, c, name);
        else if (c > t->code)
            t->right = add(t->right, c, name);
        else
            cout << "Zone code already exists.\n";

        return t;
    }

    void show(Zone* t) {
        if (t == NULL) return;
        show(t->left);
        cout << t->code << " - " << t->name << "\n";
        show(t->right);
    }

public:
    BST() { root = NULL; }

    void insert(int c, string name) {
        root = add(root, c, name);
    }

    void display() {
        show(root);
    }

    void search(int c) {
        Zone* t = root;

        while (t != NULL && t->code != c) {
            if (c < t->code) t = t->left;
            else t = t->right;
        }

        if (t != NULL) cout << "Zone: " << t->name << "\n";
        else cout << "Zone not found.\n";
    }
};

int main() {
    HashMap h;
    Queue q;
    PriorityQueue pq;
    BST bst;

    int ch;

    do {
        cout << "\n--- SMART PARCEL DELIVERY SYSTEM ---\n";
        cout << "1. Register Parcel\n";
        cout << "2. Search Parcel\n";
        cout << "3. Display Parcels\n";
        cout << "4. Process Delivery\n";
        cout << "5. Add Delivery Zone\n";
        cout << "6. Search Zone\n";
        cout << "7. Display Zones\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        cin >> ch;

        if (ch == 1) {
            Parcel p;

            cout << "Enter Parcel ID: ";
            cin >> p.id;

            if (p.id < 0 || h.search(p.id) != NULL) {
                cout << "Invalid or duplicate ID.\n";
                continue;
            }

            cin.ignore(1000, '\n');
            cout << "Enter Customer Name: ";
            getline(cin, p.customer);

            cout << "Enter Destination: ";
            getline(cin, p.destination);

            cout << "Priority (1-Normal, 2-Urgent, 3-Emergency): ";
            cin >> p.priority;

            if (p.priority < 1 || p.priority > 3) {
                cout << "Invalid priority.\n";
                continue;
            }

            p.status = "Pending";

            bool ok;
            if (p.priority == 1)
                ok = q.add(p.id);
            else
                ok = pq.add(p.id, p.priority);

            if (!ok) {
                cout << "Delivery queue is full.\n";
                continue;
            }

            h.insert(p);
            cout << "Parcel registered successfully.\n";
        }

        else if (ch == 2) {
            int id;
            cout << "Enter Parcel ID: ";
            cin >> id;

            Parcel* p = h.search(id);

            if (p != NULL) {
                cout << "Customer: " << p->customer << "\n";
                cout << "Destination: " << p->destination << "\n";
                cout << "Status: " << p->status << "\n";
            } else {
                cout << "Parcel not found.\n";
            }
        }

        else if (ch == 3) {
            h.display();
        }

        else if (ch == 4) {
            int id;
            bool done = false;

            while (pq.remove(id)) {
                Parcel* p = h.search(id);

                if (p != NULL && p->status == "Pending") {
                    p->status = "Delivered";
                    cout << "Delivered parcel: " << id << "\n";
                    done = true;
                    break;
                }
            }

            if (!done) {
                while (q.remove(id)) {
                    Parcel* p = h.search(id);

                    if (p != NULL && p->status == "Pending") {
                        p->status = "Delivered";
                        cout << "Delivered parcel: " << id << "\n";
                        done = true;
                        break;
                    }
                }
            }

            if (!done)
                cout << "No pending parcels.\n";
        }

        else if (ch == 5) {
            int code;
            string name;

            cout << "Enter Zone Code: ";
            cin >> code;
            cin.ignore(1000, '\n');

            cout << "Enter Zone Name: ";
            getline(cin, name);

            bst.insert(code, name);
        }

        else if (ch == 6) {
            int code;
            cout << "Enter Zone Code: ";
            cin >> code;
            bst.search(code);
        }

        else if (ch == 7) {
            bst.display();
        }

        else if (ch == 0) {
            cout << "Thank you!\n";
        }

        else {
            cout << "Invalid choice.\n";
        }

    } while (ch != 0);

    return 0;
}