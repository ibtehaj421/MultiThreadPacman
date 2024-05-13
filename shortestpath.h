#include<queue>
#include<unordered_map>
#include<iostream>
using namespace std;


int CELL_SIZE=20;

const int GRID_ROWS= 30;
const int GRID_COLS= 25;


struct Node_d {
    int row;
    int col;
    int distance;

    bool operator<(const Node_d& other) const {
        return distance > other.distance;
    }
};

template<class T>
class Node_Q {
public:
	Node_Q<T>* next;
	T row;
	T col;
	int count;
	Node_Q(T row, T col) {
		this->row = row;
		this->col = col;
		next = nullptr;
		count = 0;
	}
};

template<class T>
class Queue {
public:
	Node_Q<T>* front;
	Node_Q<T>* rear;
	int length;

	Queue() {
		front = nullptr;
		rear = nullptr;
		length = 0;
	}

	void enqueue(T row, T col) {
		length++;
		if (front == nullptr && rear == nullptr) {
			front = new Node_Q<T>(row, col);
			rear = front;
			return;
		}
		int count = rear->count;
		rear->next = new Node_Q<T>(row, col);
		rear = rear->next;
		rear->count = count + 1;
		return;
	}

	void dequeue() {
		if (front != nullptr && rear != nullptr) {
			length--;
			Node_Q<T>* temp = front;
			front = front->next;
			if (front == nullptr) {
				rear = nullptr;
			}
			delete temp;
		}
	}

	T peek() {
		return front->data;
	}

	T* getData(int ind) {
		Node_Q<T>* curr = front;
		while (curr->next != nullptr) {
			if (curr->count == ind) {
				break;
			}
			curr = curr->next;
		}
		T* data = new T[2];
		data[0] = curr->row;
		data[1] = curr->col;
		return data;
	}

	void reverse(Node_Q<T>* front, Node_Q<T>* rear) {
		Node_Q<T>* temp1 = front;
		Node_Q<T>* temp2 = nullptr;
		while (temp1 != temp2) {
			while (temp1->next != temp2) {
				temp1 = temp1->next;
			}
			enqueue(temp1->row, temp1->col);
			temp2 = temp1;
			temp1 = front;
		}
	}

	bool is_empty() {
		if (front == nullptr && rear == nullptr) {
			return true;
		}
		return false;
	}

	int size() {
		return length;
	}

	void clear() {
		while (front) {
			dequeue();
		}
	}
};


Queue<int> FindShortestPath(int* start, int* end) {
    priority_queue<Node_d> pq;
    unordered_map<int, pair<int, int>> parent;
    bool** visited = new bool* [GRID_ROWS];

    for (int i = 0; i < GRID_ROWS; i++) {
        visited[i] = new bool[GRID_COLS] {false};
    }

    pq.push({ start[0], start[1], 0 });

    parent[start[0] * GRID_COLS + start[1]] = { -1, -1 };

    int directions[4][2] = {
        {-1, 0},
        {1, 0},
        {0, -1},
        {0, 1}
    };

    while (!pq.empty()) {
        Node_d current = pq.top();
        pq.pop();
        if (current.row < 0 || current.row >= GRID_ROWS || current.col < 0 || current.col >= GRID_COLS) {
            continue;
        }
        if (visited[current.row][current.col]) continue;

        visited[current.row][current.col] = true;
        if (current.row == end[0] && current.col == end[1]) {
            Queue<int> path;
            while (current.row != -1 && current.col != -1) {
                path.enqueue(current.row, current.col);
                auto parentNode = parent[current.row * GRID_COLS + current.col];
                current = { parentNode.first, parentNode.second, 0 };
            }
            Queue<int> newPath;
            newPath.reverse(path.front, path.rear);
            return newPath;
        }

        for (int i = 0; i < 4; i++) {
            int newRow = current.row + directions[i][0];
            int newCol = current.col + directions[i][1];

            if (newRow >= 0 && newRow < GRID_ROWS && newCol >= 0 && newCol < GRID_COLS) {
                if (!visited[newRow][newCol] && Grid[newRow][newCol] != 1) {
                    pq.push({ newRow, newCol, current.distance + 1 });
                    parent[newRow * GRID_COLS + newCol] = { current.row, current.col };
                }
            }
        }
    }
    return {};
}

void move_enemy(Queue<int>& ghostPath, Sprite& ghost, Clock& c1) {
    int i = 0;
    int j = 0;
    if (ghostPath.front) {
        i = ghostPath.front->row;
        j = ghostPath.front->col;
    }
    ghostPath.dequeue();
    if (ghostPath.front) {
        if (c1.getElapsedTime().asMilliseconds() >= 300) {
            ghost.setPosition((float)(ghostPath.front->col * CELL_SIZE), (float)(ghostPath.front->row * CELL_SIZE));
            c1.restart();
        }
    }
}