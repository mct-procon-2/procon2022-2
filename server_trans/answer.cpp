#include<cstdlib>
#include<bits/stdc++.h>
#include<ctime>
#include<unordered_map>
#include<cassert>
#include <omp.h> 

using namespace std;

#define INF 1000000007
#define LINF (1LL << 62)
#define MOD 998244353
#define PIE 3.14159265358979
typedef long long i64;
typedef pair<i64, i64> P;
inline i64 mod(i64 a, i64 m) { return (a % m + m) % m; }
template<class T>bool chmax(T& a, const T& b) { if (a <= b) { a = b; return 1; } return 0; }
template<class T>bool chmin(T& a, const T& b) { if (b <= a) { a = b; return 1; } return 0; }

i64 ceil_div(i64 a, i64 b) { return (a + b - 1) / b; }
mt19937_64 rnd(time(NULL));



int main()
{
	string s[88], S[88];
	char EJ[2] = {'E', 'J'}, ej[2] = {'e', 'j'};
	for (int i = 0; i < 2; i++)
	{
		for (int j = 1; j <= 44; j++)
		{
			s[i * 44 + j - 1] += ej[i];
			s[i * 44 + j - 1] += (char)('0' + (j / 10));
			s[i * 44 + j - 1] += (char)('0' + (j % 10));
			S[i * 44 + j - 1] += EJ[i];
			S[i * 44 + j - 1] += (char)('0' + (j / 10));
			S[i * 44 + j - 1] += (char)('0' + (j % 10));
		}
	}
	int exist[88] = {};
	ifstream ifs("result.txt");
	for (int i = 0; i < 88; i++)
	{
		ifs >> exist[i];
	}
	ifs.close();

	while (true)
	{
		cout << "Œ»Ý:[";
		for (int i = 0; i < 88; i++)
		{
			if (exist[i])
			{
				cout << S[i] << ", ";
			}
		}
		cout << "]" << endl;
		cout << "’Ç‰Á‚·‚é(\"E01\"‚Ì‚æ‚¤‚É)‚©AÁ‚·(\"-E01\"‚Ì‚æ‚¤‚É)‚©A\"exit\" > ";
		string str;
		cin >> str;
		if (str == "exit" || str == "EXIT")
		{
			break;
		}
		if (str[0] == '-')
		{
			for (int i = 0; i < 88; i++)
			{
				if (str.substr(1, 3) == s[i] || str.substr(1, 3) == S[i])
				{
					exist[i] = 0;
				}
			}
		}
		else
		{
			for (int i = 0; i < 88; i++)
			{
				if (str == s[i] || str == S[i])
				{
					exist[i] = 100;
				}
			}
		}
		ofstream ofs("result.txt");
		for (int i = 0; i < 88; i++)
		{
			ofs << exist[i] << " ";
		}
		ofs << endl;
		ofs.close();
	}

	return 0;
}