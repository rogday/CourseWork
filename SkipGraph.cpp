#include <cassert>
#include <cmath>
#include <iostream>
#include <list>
#include <memory>
#include <random>
#include <vector>

template <class T> struct Node {
	std::unique_ptr<T> val;
	Node<T> *left;
	Node<T> *right;
	Node<T> *down;

	Node() : val(nullptr), left(nullptr), right(), down(nullptr){};
	Node(T value) : Node() { val = std::make_unique<T>(value); };
};

template <class T> class SkipGraph {
	static std::random_device rd;

  private:
	int n;
	std::vector<Node<T>> vec;

	std::mt19937 gen;
	std::uniform_int_distribution<> d;

  public:
	SkipGraph(int num)
		: n(std::ceil(log2(num))), vec(std::pow(2, n) - 1), gen(rd()),
		  d(0, std::pow(2, n - 1) - 1) {

		for (int i = 1; i < vec.size(); ++i)
			vec[i].down = &vec[i - 1];
	}

	void insert(T val) {
		uint64_t word = d(gen), shift = 0;
		Node<T> *prev = nullptr; // it will be an error with -D_GLIBCXX_DEBUG

		for (int i = 0; i < n; ++i) {
			size_t ind = (1 << i) - 1 + ((word & shift) >> (n - i - 1));

			Node<T> *it = &vec[ind];

			while (it->right != nullptr && *it->right->val < val)
				it = it->right;

			auto save = it->right;
			it->right = new Node<T>(val);
			it->right->left = it;
			it = it->right;
			it->right = save;
			if (save)
				it->right->left = it;

			it->down = prev;
			prev = it;

			shift |= uint64_t(1) << (n - i - 2);
		}
	}

	double find() {
		double out = 0;
		size_t cnt = 0;

		for (int i = std::pow(2, n - 1) - 1; i < vec.size(); ++i)
			for (auto src = vec[i].right; src != nullptr; src = src->right) {
				for (auto dst = vec[0].right; dst != nullptr;
					 dst = dst->right) {
					++cnt;

					auto &val = dst->val;

					int lvl = n - 1;
					int len = 0;
					auto it = src;

					do {
						if (it->val > val)
							while (it->right != nullptr && it->val > val) {
								++len;
								it = it->right;
							}
						else
							while (it->left != nullptr && it->val < val) {
								++len;
								it = it->left;
							}

						if (it->val == val)
							break;

						it = it->down;
						--lvl;

					} while (lvl >= 0);

					out += len;
				}
			}

		return out / cnt;
	}

	void remove(T val) {}
};

template <class T> std::random_device SkipGraph<T>::rd;

int main(int argc, char *argv[]) {
	for (int N = 2; N < 10000; N *= 2) {
		SkipGraph<int> myList(N);

		for (int i = 0; i < N; ++i) {
			int num = rand();
			myList.insert(num);
		}

		std::cout << N << ' ' << myList.find() << std::endl;
	}

	return 0;
}