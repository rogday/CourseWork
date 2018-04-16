#include <SFML/Graphics.hpp>
#include <cmath>
#include <list>
#include <random>
#include <vector>

using namespace std;
using namespace sf;

struct Dot {
	int X;
	int Y;
};

typedef list<Dot> Node;

bool bounds(int x, int N) { return x >= 0 && x < N; }

void intializeWeights(vector<vector<Node>> &Matrix) {
	int N = Matrix.size();
	random_device rd;
	mt19937 gen(rd());

	vector<float> weights(N * N);
	for (int i = 0; i < N; ++i)
		for (int k = 0; k < N; ++k) {
			weights.clear();

			for (int x : {-1, 0, 1})
				for (int y : {-1, 0, 1}) {
					if (x != 0 || y != 0) {
						int nx = x + i;
						int ny = y + k;
						if (bounds(nx, N) && bounds(ny, N))
							Matrix[i][k].push_back({nx, ny});
					}
				}

			for (int j = 0; j < N; ++j)
				for (int m = 0; m < N; ++m) {
					if ((j > i + 1 || j < i - 1) && (m > k + 1 || m < k - 1)) {
						int d = (abs(i - j) + abs(k - m));
						weights.push_back(1.0f / (d * d));
					} else
						weights.push_back(0);
				}

			discrete_distribution<> d(weights.begin(), weights.end());
			int con = d(gen);

			Matrix[i][k].push_back({con % N, con / N});
			Matrix[con % N][con / N].push_back({i, k});
		}
}

int main(int argc, char *argv[]) {
	int N = atoi(argv[1]);
	vector<vector<Node>> Matrix(N, vector<Node>(N));

	// p=1, q=1
	intializeWeights(Matrix);

	int startPoint = atoi(argv[2]), endPoint = atoi(argv[3]);

	ContextSettings set;
	set.antialiasingLevel = 8;

	RenderWindow window(sf::VideoMode(900, 900), "Kleinberg Model",
						Style::Default, set);

	window.setFramerateLimit(1);

	list<CircleShape> circles, path;
	int padding = 900 / N;

	for (int k = 0; k < N; ++k)
		for (int i = 0; i < N; ++i) {
			CircleShape tmp(padding / 4, 30);
			tmp.setFillColor(Color::White);
			tmp.setPosition(i * padding + padding / 4,
							k * padding + padding / 4);

			circles.push_back(tmp);
		}

	VertexArray vertices(Lines);

	for (int k = 0; k < N; ++k)
		for (int i = 0; i < N; ++i) {
			for (Dot &dot : Matrix[i][k]) {
				vertices.append(Vertex(Vector2f(i * padding + padding / 2,
												k * padding + padding / 2),
									   Color::Red));
				vertices.append(Vertex(Vector2f(dot.X * padding + padding / 2,
												dot.Y * padding + padding / 2),
									   Color::Red));
			}
		}

	int currPoint = startPoint;

	int ex = (endPoint % N);
	int ey = (endPoint / N);

	CircleShape t(padding / 4, 30);
	t.setFillColor(Color::Black);
	t.setPosition((currPoint % N) * padding + padding / 4,
				  (currPoint / N) * padding + padding / 4);

	path.push_back(t);

	while (window.isOpen()) {
		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed)
				window.close();
		}

		window.clear(Color::Yellow);

		for (auto &circle : circles)
			window.draw(circle);

		if (currPoint != endPoint) {
			int cx = (currPoint % N);
			int cy = (currPoint / N);

			int xm = 0, ym = 0;
			int tmp;
			int last = (abs(cx - ex) + abs(cy - ey));

			for (auto &i : Matrix[cx][cy]) {
				tmp = (abs(i.X - ex) + abs(i.Y - ey));
				if (tmp < last) {
					last = tmp;
					xm = i.X;
					ym = i.Y;
				}
			}

			vertices.append(Vertex(Vector2f(cx * padding + padding / 2,
											cy * padding + padding / 2),
								   Color::Black));
			vertices.append(Vertex(Vector2f(xm * padding + padding / 2,
											ym * padding + padding / 2),
								   Color::Black));

			CircleShape t(padding / 4, 30);
			t.setFillColor(Color::Black);
			t.setPosition(xm * padding + padding / 4,
						  ym * padding + padding / 4);

			path.push_back(t);

			currPoint = xm + ym * N;
		}

		window.draw(vertices);

		for (auto &circle : path)
			window.draw(circle);

		window.display();
	}

	return 0;
}