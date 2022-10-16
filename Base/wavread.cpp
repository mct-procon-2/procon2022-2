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

	for (int i = 0; i < t_size - 5; i++)
	{
		int cnt = 0;
		for (int j = 1; j < 5; j++)
			cnt += wave_t[i + j];
		wave_t1[i] = cnt/4 - wave_t[i];
	}
	//for (int i = 0; i < t_size - 2; i++)
	//{
	//	wave_t2[i] = wave_t1[i + 2] - wave_t1[i];
	//}

	cout << "音声が混ざってる数>";
	cin >> max_num;
	cout << "許容するズレ（300とか）>";
	i64 range = 300;
	cin >> range;
	cout << "逆位相の範囲（24000とか）>";
	int rerange = 24000;
	cin >> rerange;

	int choiced = 0;
	vector<pair<bool, string>> miv(88);
	int mivindex = 0;
	priority_queue<pair<pair<int, int>, pair<string, int>>> er;
	char EJ[2] = { 'E', 'J' };
	for (const char& ej : EJ) for (int n = 1; n <= 44; n++)
	{
		memset(wave_s, 0, sizeof wave_s);
		memset(wave_s1, 0, sizeof wave_s1);
		memset(wave_s2, 0, sizeof wave_s2);
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

		for (int i = 0; i < s_size - Time_range; i += Time_range)
		{
			bool ok = false;
			for (int j = i; j < i + Time_range; j++)
			{
				if (abs(wave_s[j]) > 1500)
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

		for (int i = 0; i < s_size - 5; i++)
		{
			int cnt = 0;
			for (int j = 1; j < 5; j++)
			{
				cnt += wave_s[i + j];
			}
			wave_s1[i] = cnt / 4 - wave_s[i];
		}
		//for (int i = 0; i < s_size - 2; i++)
		//{
		//	wave_s2[i] = wave_s1[i + 2] - wave_s1[i];
		//}

		//類似度を見る
		{
			bool choice = false;
			int start = INF, end = -1;
			i64 mi = LINF, mi1 = LINF, mi2 = LINF;
			for (const auto& l : feaindex_s)
			{
				const int r = l + Time_range;

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
						i64 cnt = 0, cnt1 = 0, cnt2 = 0;
						for (int i = l; i < r; i++)
						{
							cnt += pow2(wave_s[i] - wave_t[t + i]);
							if (cnt > range * (Time_range))
							{
								cnt_ok = false;
								break;
							}
							//if (cnt >= mi) continue;
						}
						//for (int i = l; i < r-5; i++)
						//{
						//	cnt1 += pow2(wave_s1[i] - wave_t1[t + i]);
						//	if (cnt1 > range * (Time_range - 5))
						//	{
						//		cnt_ok = false;
						//		break;
						//	}
						//	//int a = (wave_s1[i] >= 1 ? 1 : (wave_s1[i] <= -1 ? -1 : 0));
						//	//int b = (wave_t1[t + i] >= 1 ? 1 : (wave_t1[t + i] <= -1 ? -1 : 0));
						//	//if (a != b)
						//	//{
						//	//	cnt1++;
						//	//	if (cnt1 > range)
						//	//	{
						//	//		cnt_ok = false;
						//	//		break;
						//	//	}
						//	//}
						//	//if (cnt1 >= mi1) continue;
						//}
						//for (int i = l; i < r - 2; i++)
						//{
						//	cnt2 += pow2(wave_s2[i] - wave_t2[t + i]);
						//	if (cnt2 > range * (Time_range - 2))
						//	{
						//		cnt_ok = false;
						//		break;
						//	}
						//}
						if (cnt_ok)
						//if (cnt <= range * (Time_range))
						{
							//cout << cnt << " " << cnt1 << " " << cnt2 << endl;
							cout << cnt << endl;
							chmin(st, t + l);
							ok = true;
						}
						chmin(mi, cnt);
						//if (chmin(mi, cnt))
						//{
						//	cout << cnt << " " << cnt1 << endl;
						//}
						//if (chmin(mi1, cnt1))
						//{
						//	cout << cnt << " " << cnt1 << endl;
						//}
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
							i64 cnt = 0, cnt1 = 0, cnt2 = 0;
							for (int i = l + t; i < r + t; i++)
							{
								cnt += pow2(wave_s[i] - wave_t[start - l + i]);
								if (cnt > range * (Time_range))
								{
									cnt_ok = false;
									break;
								}
							}
							//for (int i = l + t; i < r + t - 5; i++)
							//{
							//	cnt1 += pow2(wave_s1[i] - wave_t1[start - l + i]);
							//	if (cnt1 > range * (Time_range - 5))
							//	{
							//		cnt_ok = false;
							//		break;
							//	}
							//	//int a = (wave_s1[i] >= 1 ? 1 : (wave_s1[i] <= -1 ? -1 : 0));
							//	//int b = (wave_t1[start - l + i] >= 1 ? 1 : (wave_t1[start - l + i] <= -1 ? -1 : 0));
							//	//if (a != b)
							//	//{
							//	//	cnt1++;
							//	//	if (cnt1 > range)
							//	//	{
							//	//		cnt_ok = false;
							//	//		break;
							//	//	}
							//	//}
							//}
							//for (int i = l + t; i < r + t - 2; i++)
							//{
							//	cnt2 += pow2(wave_s2[i] - wave_t2[start - l + i]);
							//	if (cnt2 > range * (Time_range - 2))
							//	{
							//		cnt_ok = false;
							//		break;
							//	}
							//}
							if (cnt_ok)
							//if (cnt <= range * (Time_range))
							{
								//cout << cnt << endl;
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
			cout << mi << " " << mi1 << endl;
			mis.push_back({ mi, target });
			mis1.push_back({ mi1, target });
			cout << choice << endl;
			cout << endl;
			if (choiced == max_num)
				goto finish;
		}
	}
	sort(mis.begin(), mis.end());
	sort(mis1.begin(), mis1.end());

	cout << "\n0:sort" << endl;
	for (const auto ab : mis)
	{
		cout << ab.first << " " << ab.second << endl;
	}
	cout << "\n1:sort" << endl;
	for (const auto ab : mis1)
	{
		cout << ab.first << " " << ab.second << endl;
	}

	return 0;

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

				
				for (int i = start_v - 1; i >= start_v - rerange; i--)
				{
					if (i < 0) break;
					if (i - start_v + l_v < 0) break;
					wave_t[i] -= wave_s[i - start_v + l_v];
				}
				for (int i = start_v; i < end_v; i++)
				{
					wave_t[i] -= wave_s[i - start_v + l_v];
				}
				for (int i = end_v + 1; i < end_v + rerange; i++)
				{
					if (i >= t_size) break;
					if (i - start_v + l_v >= s_size) break;
					wave_t[i] -= wave_s[i - start_v + l_v];
				}
			}

			for (int i = 0; i < t_size - 1; i++)
			{
				int cnt = 0;
				for (int j = 1; j < 5; j++)
				{
					cnt += wave_t[i + j];
				}
				wave_t1[i] = cnt / 4 - wave_t[i];
			}
			//for (int i = 0; i < t_size - 2; i++)
			//{
			//	wave_t2[i] = wave_t1[i + 1] - wave_t1[i];
			//}

			//探す
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
					memset(wave_s1, 0, sizeof wave_s1);
					memset(wave_s2, 0, sizeof wave_s2);
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

					for (int i = 0; i < s_size - Time_range; i += Time_range)
					{
						bool ok = false;
						for (int j = i; j < i + Time_range; j++)
						{
							if (abs(wave_s[i]) > 1500)
							{
								ok = true;
								break;
							}
						}
						if (!ok) continue;
						feaindex_s.emplace_back(i);
					}

					for (int i = 0; i < s_size - 5; i++)
					{
						int cnt = 0;
						for (int j = 1; j < 5; j++)
						{
							cnt += wave_s[i + j];
						}
						wave_s1[i] = cnt / 4 - wave_s[i];
					}
					//for (int i = 0; i < s_size - 2; i++)
					//{
					//	wave_s2[i] = wave_s1[i + 1] - wave_s1[i];
					//}

					//類似度を見る
					{
						bool choice = false;
						int start = INF, end = -1;
						i64 mi = LINF, mi1 = LINF;
						for (const auto& l : feaindex_s)
						{
							const int r = l + Time_range;
#pragma omp parallel
							{
								bool ok = false;
								int st = INF;
#pragma omp for
								for (int t = t_size - r; t >= -l; t--)
								{
									if (ok) continue;

									bool cnt_ok = true;
									i64 cnt = 0, cnt1 = 0, cnt2 = 0;
									for (int i = l; i < r; i++)
									{
										cnt += pow2(wave_s[i] - wave_t[t + i]);
										if (cnt > range * (Time_range))
										{
											cnt_ok = false;
											break;
										}
									}
									for (int i = l; i < r-5; i++)
									{
										cnt1 += pow2(wave_s1[i] - wave_t1[t + i]);
										if (cnt1 > range * (Time_range - 5))
										{
											cnt_ok = false;
											break;
										}
										//int a = (wave_s1[i] >= 1 ? 1 : (wave_s1[i] <= -1 ? -1 : 0));
										//int b = (wave_t1[t + i] >= 1 ? 1 : (wave_t1[t + i] <= -1 ? -1 : 0));
										//if (a != b)
										//{
										//	cnt1++;
										//	if (cnt1 > range)
										//	{
										//		cnt_ok = false;
										//		break;
										//	}
										//}
									}
									//for (int i = l; i < r - 2; i++)
									//{
									//	cnt2 += pow2(wave_s2[i] - wave_t2[t + i]);
									//	if (cnt2 > range * (Time_range - 2))
									//	{
									//		cnt_ok = false;
									//		break;
									//	}
									//}
									if (cnt_ok)
									//if(cnt <= range * (r-l))
									{
										//cout << cnt << " " << cnt1 << " " << cnt2 << endl;
										//cout << cnt << endl;
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
										i64 cnt = 0, cnt1 = 0, cnt2 = 0;
										for (int i = l + t; i < r + t; i++)
										{
											cnt += pow2(wave_s[i] - wave_t[start - l + i]);
											if (cnt > range * (Time_range))
											{
												cnt_ok = false;
												break;
											}
										}
										for (int i = l + t; i < r + t-5; i++)
										{
											cnt1 += pow2(wave_s1[i] - wave_t1[start - l + i]);
											if (cnt1 > range * (Time_range-5))
											{
												cnt_ok = false;
												break;
											}
											//int a = (wave_s1[i] >= 1 ? 1 : (wave_s1[i] <= -1 ? -1 : 0));
											//int b = (wave_t1[start - l + i] >= 1 ? 1 : (wave_t1[start - l + i] <= -1 ? -1 : 0));
											//if (a != b)
											//{
											//	cnt1++;
											//	if (cnt1 > range)
											//	{
											//		cnt_ok = false;
											//		break;
											//	}
											//}
										}
										//for (int i = l + t; i < r + t - 2; i++)
										//{
										//	cnt2 += pow2(wave_s2[i] - wave_t2[start - l + i]);
										//	if (cnt2 > range * (Time_range - 2))
										//	{
										//		cnt_ok = false;
										//		break;
										//	}
										//}
										if (cnt_ok)
										//if (cnt <= range * (Time_range))
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
						//cout << mi << " " << mi1 << endl;
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