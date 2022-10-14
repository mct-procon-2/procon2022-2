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
	string headname = "problems/", probname;
	cout << "音声ファイル名 > problems/";
	cin >> probname;
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
	cout << "許容するズレ（1000とか）>";
	i64 range = 1000;
	cin >> range;

	int choiced = 0;
	vector<pair<bool, string>> miv(88);
	int mivindex = 0;
	priority_queue<pair<pair<int, int>, pair<string, int>>> er;
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
				if (abs(wave_s[i]) > 1000)
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


		//類似度を見る
		{
			bool choice = false;
			int start = INF, end = -1;
			for (const auto& l : feaindex_s)
			{
				const int r = l + 200;
				//cout << r << endl;
#pragma omp parallel
				{
					bool ok = false;
					int st = INF;
#pragma omp for
					for (int t = -l; t < t_size - r; t++)
					{
						if (ok) continue;
						bool cnt_ok = true;
						i64 cnt = 0;
						for (int i = l; i < r; i++)
						{
							cnt += (wave_s[i] - wave_t[t + i]) * (wave_s[i] - wave_t[t + i]);
							if (cnt > range * (r - l))
							{
								cnt_ok = false;
								break;
							}
						}
						if (cnt_ok)
						//if (cnt <= range * (r - l))
						{
							chmin(st, t + l);
							ok = true;
						}
					}
#pragma omp critical
					{
						if (ok)
						{
							choiced++;
							choice = true;
							chmin(start, st);
						}
					}
				}

				if (choice)
				{
#pragma omp parallel
					{
						bool ok = false;
						int en = -1;
#pragma omp for
						for (int t = min(t_size - start + l - r, s_size - r); t >= max(-l, -start); t--)
						{
							if (ok) continue;
							bool cnt_ok = true;
							i64 cnt = 0;
							for (int i = l + t; i < r + t; i++)
							{
								cnt += (wave_s[i] - wave_t[start - l + i]) * (wave_s[i] - wave_t[start - l + i]);
								if (cnt > range * (r - l))
								{
									cnt_ok = false;
									break;
								}
							}
							if (cnt_ok)
							//if (cnt <= range * (r - l))
							{
								cout << cnt << endl;
								chmax(en, start - l + r + t);
								ok = true;
							}
						}
#pragma omp critical
						{
							if (ok)
							{
								chmax(end, en);
							}
						}
					}
					er.push({ {start, end}, {target, l} });
					break;
				}
			}
			miv[mivindex] = { choice, target };
			mivindex++;
			cout << choice << endl;
			cout << endl;
			if (choiced == max_num)
				goto finish;
		}
	}


	if (choiced < max_num)
	{
		while (!er.empty())
		{

			//消す
			while (!er.empty())
			{
				//cout << "erase" << endl;
				auto v = er.top();
				er.pop();
				int start_v = v.first.first, end_v = v.first.second;
				string target_v = v.second.first;
				int l_v = v.second.second;
				cout << start_v << " " << end_v << endl;

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

				
				for (int i = start_v - 1; i >= start_v - 2400; i--)
				{
					if (i < 0) break;
					if (i - start_v + l_v < 0) break;
					wave_t[i] -= wave_s[i - start_v + l_v];
				}
				for (int i = start_v; i < end_v; i++)
				{
					wave_t[i] -= wave_s[i - start_v + l_v];
				}
				for (int i = end_v + 1; i < end_v + 2400; i++)
				{
					if (i >= t_size) break;
					if (i - start_v + l_v >= s_size) break;
					wave_t[i] -= wave_s[i - start_v + l_v];
				}
			}

			//探す
			{
				//cout << "search" << endl;
				mivindex = 0;
				for (const char& ej : EJ) for (int n = 1; n <= 44; n++)
				{
					if (!(mivindex == n - 1 + 44 || mivindex == n - 1))
					{
						cout << "?" << endl;
						exit(0);
					}
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
							if (abs(wave_s[i]) > 1000)
							{
								ok = true;
								break;
							}
						}
						if (!ok) continue;
						feaindex_s.emplace_back(i);
					}


					//類似度を見る
					{
						bool choice = false;
						int start = INF, end = -1;
						for (const auto& l : feaindex_s)
						{
							const int r = l + 200;
#pragma omp parallel
							{
								bool ok = false;
								int st = INF;
#pragma omp for
								for (int t = t_size - r; t >= -l; t--)
								{
									if (ok) continue;

									bool cnt_ok = true;
									i64 cnt = 0;
									for (int i = l; i < r; i++)
									{
										cnt += (wave_s[i] - wave_t[t + i]) * (wave_s[i] - wave_t[t + i]);
										if (cnt > range * (r - l))
										{
											cnt_ok = false;
											break;
										}
									}
									if (cnt_ok)
									//if(cnt <= range * (r-l))
									{
										chmin(st, t + l);
										ok = true;
									}
								}
#pragma omp critical
								{
									if (ok)
									{
										choiced++;
										choice = true;
										chmin(start, st);
									}
									
								}
							}
							if (choice)
							{
#pragma omp parallel
								{
									bool ok = false;
									int en = -1;
#pragma omp for
									for (int t = min(t_size - start + l - r, s_size - r); t >= max(-l, -start); t--)
									{
										if (ok) continue;

										bool cnt_ok = true;
										i64 cnt = 0;
										for (int i = l + t; i < r + t; i++)
										{
											cnt += (wave_s[i] - wave_t[start - l + i]) * (wave_s[i] - wave_t[start - l + i]);
											if (cnt > range * (r - l))
											{
												cnt_ok = false;
												break;
											}
										}
										if (cnt_ok)
										//if (cnt <= range * (r - l))
										{
											chmax(en, start - l + r + t);
											ok = true;
										}
									}
#pragma omp critical
									{
										if (ok)
										{
											chmax(end, en);
										}
									}
								}
								er.push({ {start, end}, {target, l} });
								break;
							}
						}
						miv[mivindex] = { choice, target };
						mivindex++;
						cout << choice << endl;
						cout << endl;
						if (choiced == max_num)
							goto finish;
					}
				}
			}//探すend
		}
	}

finish:
	//出力
	for (int i = 0; i < 88; i++)
	{
		if (miv[i].first)
			cout << miv[i].second << " ";
	}
	cout << endl;

	ofstream ofs("result.txt");
	for (int i = 0; i < 88; i++)
	{
		if (miv[i].first)
			ofs << 100 << " ";
		else
			ofs << 0 << " ";
	}
	ofs << endl;

	cout << "time:" << (double)(clock() - start_t) / CLOCKS_PER_SEC << endl;

	return 0;
}