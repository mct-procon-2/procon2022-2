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




char nodata[44];
short wave_t[1010101], wave_s[1010101];
vector<int> feaindex_t;
int main()
{
	int max_num = 20;
	string probname = "./wave_file/mix0.wav";
	cout << "‰¹ºƒtƒ@ƒCƒ‹–¼>";
	cin >> probname;
	cout << "‰¹º‚ª¬‚´‚Á‚Ä‚é”>";
	cin >> max_num;

	clock_t start_t = clock();
	FILE* fp;
	fp = fopen(probname.data(), "rb");	//–â‘èƒtƒ@ƒCƒ‹
	if (fp == NULL)
	{
		cout << "ERROR" << endl;
		return -1;
	}
	fread(nodata, 1, 44, fp);
	int t_size = fread(wave_t, 2, 1010101, fp);
	//cout << t_size << endl;
	fclose(fp);

	int choiced = 0;
	vector<pair<bool, string>> miv(88);
	int mivindex = 0;
	priority_queue<pair<int, pair<string, int>>> er;
	char EJ[2] = { 'E', 'J' };
	for (const char& ej : EJ) for (int n = 1; n <= 44; n++)
	{
		memset(wave_s, 0, sizeof wave_s);
		vector<int> feaindex_s;
		string target;
		target += ej;
		target += (char)('0' + (n / 10));
		target += (char)('0' + (n % 10));
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
		//cout << s_size << endl;
		fclose(fp);

		for (int i = 0; i < s_size - 200; i += 200)
		{
			bool ok = false;
			for (int j = i; j < i + 200; j++)
			{
				if (abs(wave_s[i]) > 300)
				{
					ok = true;
					break;
				}
			}
			if (!ok) continue;
			feaindex_s.emplace_back(i);
		}
		int fs_size = feaindex_s.size();
		//cout << fs_size << endl;


		//ˆê’v‚·‚é‚©‚ðŒ©‚é
		{
			bool choice = false;
			int start = -1;
			for (const auto& l : feaindex_s)
			{
				const int r = l + 200;
				//cout << r << endl;
#pragma omp parallel
				{
					bool ok = false;
#pragma omp for
					for (int t = -l; t < t_size - r; t++)
					{
						if (ok) continue;
						bool okk = true;
						for (int i = l; i < r; i++)
						{
							if (wave_s[i] == wave_t[t + i])
								continue;
							okk = false;
							break;
						}
						if (okk)
						{
							start = t + l;
							ok = true;
						}
					}
#pragma omp critical
					{
						if (ok)
							choice = true;
					}
				}
				if (choice)
				{
					er.push({ start, {target, l} });
					choiced++;
					break;
				}
			}
			miv[mivindex] = { choice, target };
			mivindex++;
			cout << choice << endl;
		}
		cout << endl;
	}


	if(choiced < max_num) 
	{
		while (!er.empty())
		{

			//Á‚·
			while(!er.empty()) {
				//cout << "erase" << endl;
				auto v = er.top();
				er.pop();
				int start_v = v.first;
				string target_v = v.second.first;
				int l_v = v.second.second;

				memset(wave_s, 0, sizeof wave_s);
				string filename = "./JKspeech/" + target_v + ".wav";
				fp = fopen(filename.data(), "rb");
				if (fp == NULL)
				{
					cout << "ERROR" << endl;
					return -1;
				}
				fread(nodata, 1, 44, fp);
				int s_size = fread(wave_s, 2, 1010101, fp);
				fclose(fp);

				int wide = 0;
				for (int i = l_v; i < l_v + wide + 24000; i++)
				{
					if (i < 0) continue;
					if (start_v + i - l_v < 0) continue;
					if (i >= s_size) break;
					if (start_v + i - l_v >= t_size) break;
					wave_t[start_v + i - l_v] -= wave_s[i];
					if (wave_t[start_v + i - l_v] == 0)
					{
						wide++;
					}
				}
				wide = 0;
				for (int i = l_v - 1; i >= l_v - wide - 23800; i--)
				{
					if (i < 0) break;
					if (start_v + i - l_v < 0) break;
					if (i >= s_size) continue;
					if (start_v + i - l_v >= t_size) continue;
					wave_t[start_v + i - l_v] -= wave_s[i];
					if (wave_t[start_v + i - l_v] == 0)
					{
						wide++;
					}
				}
			}

			//’T‚·
			{
				//cout << "search" << endl;
				mivindex = 0;
				for (const char& ej : EJ) for (int n = 1; n <= 44; n++)
				{
					if (miv[n - 1].first || miv[n - 1 + 44].first)
					{
						mivindex++;
						continue;
					}
					memset(wave_s, 0, sizeof wave_s);
					vector<int> feaindex_s;
					string target;
					target += ej;
					target += (char)('0' + (n / 10));
					target += (char)('0' + (n % 10));
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

					for (int i = 0; i < s_size - 200; i += 200)
					{
						bool ok = false;
						for (int j = i; j < i + 200; j++)
						{
							if (abs(wave_s[i]) > 300)
							{
								ok = true;
								break;
							}
						}
						if (!ok) continue;
						feaindex_s.emplace_back(i);
					}


					//ˆê’v‚·‚é‚©‚ðŒ©‚é
					{
						bool choice = false;
						int start = -1;
						for (const auto& l : feaindex_s)
						{
							const int r = l + 200;
#pragma omp parallel
							{
								bool ok = false;
#pragma omp for
								for (int t = -l; t < t_size - r; t++)
								{
									if (ok) continue;
									bool okk = true;
									for (int i = l; i < r; i++)
									{
										if (wave_s[i] == wave_t[t + i])
											continue;
										okk = false;
										break;
									}
									if (okk)
									{
										start = t + l;
										ok = true;
									}
								}
#pragma omp critical
								{
									if (ok)
										choice = true;
								}
							}
							if (choice)
							{
								er.push({ start, {target, l} });
								choiced++;
								break;
							}
						}
						cout << choice << endl;
						cout << endl;
						miv[mivindex] = { (choice ? -1 : 0), target };
						mivindex++;
						if (choiced == max_num)
							goto finish;
					}
				}
			}//’T‚·end
		}
	}

	finish:
	//o—Í
	for (int i = 0; i < 88; i++)
	{
		if(miv[i].first)
			cout << miv[i].second << " ";
	}
	cout << endl;
	cout << "time:" << (double)(clock() - start_t) / CLOCKS_PER_SEC << endl;

	return 0;
}