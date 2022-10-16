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


#define Time_range 200

inline i64 pow2(i64 a) { return a * a; }

char nodata[44];
short wave_t[1010101], wave_s[1010101];
short wave_s1[1010101], wave_t1[1010101];
short wave_s2[1010101], wave_t2[1010101];
vector<int> feaindex_t;
vector<pair<int, string>> mis, mis1;
i64 wtsum[1010101];
int main()
{
	int max_num = 20;
	string headname = "problems/", probname;
	cout << "音声ファイル名 > problems/";
	cin >> probname;
	if (probname.substr(probname.size() - 4, 4) != ".wav")
		probname += ".wav";
	probname = headname + probname;

	clock_t start_t = clock();
	FILE* fp;
	fp = fopen(probname.data(), "rb");	//問題ファイル
	if (fp == NULL)
	{
		cout << "ERROR" << endl;
		return -1;
	}
	fread(nodata, 1, 44, fp);
	int t_size = fread(wave_t, 2, 1010101, fp);
	//cout << t_size << endl;
	fclose(fp);


	cout << "音声が混ざってる数>";
	cin >> max_num;
	//cout << "許容するズレ（300とか）>";
	//i64 range = 300;
	//cin >> range;
	//cout << "逆位相の範囲（24000とか）>";
	//int rerange = 24000;
	//cin >> rerange;

	int choiced = 0;
	vector<bool> answer(88);
	char EJ[2] = { 'E', 'J' };
	string targets[88];
	for (int i = 0; i < 2; i++) for (int n = 1; n <= 44; n++)
	{
		targets[i*44 + n-1] += EJ[i];
		targets[i * 44 + n-1] += (char)('0' + (n / 10));
		targets[i * 44 + n-1] += (char)('0' + (n % 10));
	}

	for (int i = 0; i < t_size; i++)
	{
		wtsum[i + 1] = wtsum[i] + abs(wave_t[i]);
	}
	
	int pnt[88] = {};
	vector<pair<i64, pair<int,int>>> v(88, { LINF, {0,0} });
	int choicenum = 0;
	while (choicenum < max_num)
	{
		i64 ami = LINF, ak = -1;
		int start = -1;
		for (int k = 0; k < 88; k++)
		{
			if (answer[k]) continue;
			vector<pair<int, short>> feature;
			memset(wave_s, 0, sizeof wave_s);
			string target = targets[k];
			cout << target << endl;

			string filename = "./JKspeech/" + target + ".wav";
			fp = fopen(filename.data(), "rb");
			if (fp == NULL)
			{
				cout << "ERROR" << endl;
				return -1;
			}
			fread(nodata, 1, 44, fp);
			int s_size = fread(wave_s, 2, 1010101, fp);
			fclose(fp);


#pragma omp parallel
			{
				P mi = { LINF, -1 };
#pragma omp for
				for (int s = -s_size; s < s_size + t_size; s+=3)
				//for (int s = 0; s < s_size; s++)
				{
					i64 cnt = 0, mmi = LINF;
					for (int t = 0; t < t_size; t+=10)
					{
						if (s + t < 0 || s+t >= s_size)
						{
							cnt += abs(wave_t[t]);
							continue;
						}
						cnt += abs(wave_s[s+t] - wave_t[t]);
					}
					if (chmin(mi.first, cnt))
					{
						mi.second = s;
					}
				}
#pragma omp critical
				{
					chmin(v[k], { mi.first, {k, mi.second} });
				}
			}
			cout << v[k].first << endl;
		}
		cout << endl;
		
		sort(v.begin(), v.end());
		i64 avg = 0, anum = 0;
		bool check = false;
		for (const auto& vv : v)
		{
			if (answer[vv.second.first]) continue;
			if (!check)
			{
				ak = vv.second.first;
				check = true;
			}
			cout << vv.first << " " << targets[vv.second.first] << endl;
			avg += vv.first;
			anum++;
		}
		//E14 E42 J19 J31 J32 J38 J43
		//E14 E42 J19 J31 J32 J33 J38 J43
		avg /= anum;
		for (const auto& vv : v)
		{
			if (answer[vv.second.first]) continue;
			if (avg * 0.85 > vv.first || ak == vv.second.first)
			{
				if (choicenum >= max_num)
				{
					break;
				}
				answer[vv.second.first] = true;
				choicenum++;
				//逆位相
				memset(wave_s, 0, sizeof wave_s);
				string target = targets[vv.second.first];
				cout << target << endl;

				string filename = "./JKspeech/" + target + ".wav";
				fp = fopen(filename.data(), "rb");
				if (fp == NULL)
				{
					cout << "ERROR" << endl;
					return -1;
				}
				fread(nodata, 1, 44, fp);
				int s_size = fread(wave_s, 2, 1010101, fp);
				fclose(fp);
				for (int i = max(0, -vv.second.second); i < t_size; i++)
				{
					if (i + vv.second.second >= s_size) break;
					wave_t[i] -= wave_s[i + vv.second.second];
				}
			}
		}

		//出力
		for (int i = 0; i < 88; i++)
		{
			if (answer[i])
				cout << targets[i] << " ";
		}
		cout << endl;

		ofstream ofs("result.txt");
		for (int i = 0; i < 88; i++)
		{
			if (answer[i])
				ofs << 100 << " ";
			else
				ofs << 0 << " ";
		}
		ofs << endl;

		i64 sum = 0;
		for (int i = 0; i < t_size; i++)
		{
			sum += abs(wave_t[i]);
		}
		cout << "sum:" << sum << endl;

		cout << "time:" << (double)(clock() - start_t) / CLOCKS_PER_SEC << endl;
	}

finish:
	//出力
	for (int i = 0; i < 88; i++)
	{
		if (answer[i])
			cout << targets[i] << " ";
	}
	cout << endl;

	ofstream ofs("result.txt");
	for (int i = 0; i < 88; i++)
	{
		if (answer[i])
			ofs << 100 << " ";
		else
			ofs << 0 << " ";
	}
	ofs << endl;

	cout << "time:" << (double)(clock() - start_t) / CLOCKS_PER_SEC << endl;

	return 0;
}