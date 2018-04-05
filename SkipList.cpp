#include <cmath>
#include <iostream>
#include <list>
#include <memory>
#include <random>
#include <vector>

template <class T> struct Node {
	std::unique_ptr<T> val;
	Node<T> *right;
	Node<T> *down;

	Node() : val(nullptr), right(nullptr), down(nullptr){};
	Node(T value) : Node() { val = std::make_unique<T>(value); };
};

template <class T> class SkipList {
	static std::random_device rd;

  private:
	int n;
	int m;
	std::vector<Node<T> *> vec;

	std::mt19937 gen;
	std::discrete_distribution<> d;

  public:
	SkipList(double p, int n)
		: n(n), m(log(n) / log(1.0 / p) + 0.5), vec(m), gen(rd()),
		  d({1 - p, p}) {

		vec[m - 1] = new Node<T>();
		vec[m - 1]->down = nullptr;

		for (int i = m - 2; i >= 0; --i) {
			vec[i] = new Node<T>();
			vec[i]->down = vec[i + 1];
		}
	}

	void insert(T val) {
		auto origin = vec[0];
		std::list<Node<T> *> pointers;

		for (int k = 0; k < m; ++k) {
			for (auto i = origin->right; i != nullptr; origin = i, i = i->right)
				if (val < *(i->val))
					break;

			pointers.push_back(origin);
			origin = origin->down;
		}

		auto pos = pointers.rbegin();
		auto save = (*pos)->right;
		(*pos)->right = new Node<T>(val);
		(*pos)->right->right = save;

		for (auto j = ++pointers.rbegin(), i(j--); i != pointers.rend();
			 j = i, ++i)
			if (d(gen)) {
				auto save = (*i)->right;
				(*i)->right = new Node<T>(val);
				(*i)->right->right = save;
				(*i)->right->down = (*j)->right;
			} else
				break;
	}

	void print() {
		std::cout << m << std::endl;

		for (auto &i : vec) {
			auto ptr = i->right;
			std::cout << ptr << ": " << std::flush;

			while (ptr != nullptr) {
				std::cout << *(ptr->val) << ' ' << std::flush;
				ptr = ptr->right;
			}

			std::cout << std::endl;
		}
	}
};

template <class T> std::random_device SkipList<T>::rd;

int main() {
	int N = 100;
	SkipList<int> myList(0.5, N);

	int i = 0;
	while (++i < N)
		myList.insert(rand());

	myList.print();
	return 0;
}