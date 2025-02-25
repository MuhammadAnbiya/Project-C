#include <iostream>
#include <vector>
#include <numeric>

using namespace std;

long long gcd(long long a, long long b) {
    return b == 0 ? a : gcd(b, a % b);
}

void printFraction(long long numerator, long long denominator) {
    if (denominator < 0) {
        numerator = -numerator;
        denominator = -denominator;
    }

    long long g = gcd(abs(numerator), abs(denominator));
    cout << numerator / g << "/" << denominator / g << endl;
}

class SegmentTree {
private:
    int N;
    vector<long long> salaries;
    vector<long long> happiness;
    vector<long long> sumSalaries;
    vector<long long> sumHappiness;
    vector<long long> lazySalary;

    void build(int node, int start, int end) {
        if (start == end) {
            sumSalaries[node] = salaries[start];
            sumHappiness[node] = 0;
        } else {
            int mid = (start + end) / 2;
            build(2 * node + 1, start, mid);
            build(2 * node + 2, mid + 1, end);
            sumSalaries[node] = sumSalaries[2 * node + 1] + sumSalaries[2 * node + 2];
            sumHappiness[node] = sumHappiness[2 * node + 1] + sumHappiness[2 * node + 2];
        }
    }

    void propagate(int node, int start, int end) {
        if (lazySalary[node] != 0) {
            long long updateValue = lazySalary[node];
            sumSalaries[node] += (end - start + 1) * updateValue;

            if (start != end) {
                lazySalary[2 * node + 1] += updateValue;
                lazySalary[2 * node + 2] += updateValue;
            }
            lazySalary[node] = 0;
        }
    }

    void rangeUpdate(int node, int start, int end, int l, int r, long long value) {
        propagate(node, start, end);
        if (start > end || start > r || end < l) {
            return;
        }
        if (start >= l && end <= r) {
            lazySalary[node] += value;
            propagate(node, start, end);
            return;
        }
        int mid = (start + end) / 2;
        rangeUpdate(2 * node + 1, start, mid, l, r, value);
        rangeUpdate(2 * node + 2, mid + 1, end, l, r, value);
        sumSalaries[node] = sumSalaries[2 * node + 1] + sumSalaries[2 * node + 2];
    }

    long long rangeQuery(int node, int start, int end, int l, int r, bool isSalary) {
        propagate(node, start, end);
        if (start > end || start > r || end < l) {
            return 0;
        }
        if (start >= l && end <= r) {
            return isSalary ? sumSalaries[node] : sumHappiness[node];
        }
        int mid = (start + end) / 2;
        long long leftQuery = rangeQuery(2 * node + 1, start, mid, l, r, isSalary);
        long long rightQuery = rangeQuery(2 * node + 2, mid + 1, end, l, r, isSalary);
        return leftQuery + rightQuery;
    }

public:
    SegmentTree(vector<long long>& initial_salaries) {
        N = initial_salaries.size();
        salaries = initial_salaries;
        sumSalaries.resize(4 * N);
        sumHappiness.resize(4 * N);
        lazySalary.resize(4 * N, 0);
        build(0, 0, N - 1);
    }

    void setSalaries(int l, int r, long long value) {
        long long currentSum = rangeQuery(0, 0, N - 1, l - 1, r - 1, true);
        long long newSum = value * (r - l + 1);
        long long change = newSum - currentSum;
        rangeUpdate(0, 0, N - 1, l - 1, r - 1, change);
    }

    void addToSalaries(int l, int r, long long value) {
        rangeUpdate(0, 0, N - 1, l - 1, r - 1, value);
    }

    void querySalaryAverage(int l, int r) {
        long long totalSalary = rangeQuery(0, 0, N - 1, l - 1, r - 1, true);
        printFraction(totalSalary, r - l + 1);
    }

    void queryHappinessAverage(int l, int r) {
        long long totalHappiness = rangeQuery(0, 0, N - 1, l - 1, r - 1, false);
        printFraction(totalHappiness, r - l + 1);
    }
};

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int N, Q;
    cin >> N >> Q;

    vector<long long> initial_salaries(N);
    for (int i = 0; i < N; i++) {
        cin >> initial_salaries[i];
    }

    SegmentTree sol(initial_salaries);

    for (int i = 0; i < Q; i++) {
        int type, l, r;
        cin >> type >> l >> r;

        if (type <= 1) {
            long long c;
            cin >> c;
            if (type == 0) {
                sol.setSalaries(l, r, c);
            } else {
                sol.addToSalaries(l, r, c);
            }
        } else if (type == 2) {
            sol.querySalaryAverage(l, r);
        } else {
            sol.queryHappinessAverage(l, r);
        }
    }

    return 0;
}
