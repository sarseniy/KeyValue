﻿#include <iostream>
#include <string>
#include <vector>

template <typename iter_t>
void merge(iter_t p, iter_t q, iter_t r)
{

	int n1, n2, i, j;
	iter_t k = p;

	n1 = q - p + 1;
	n2 = r - q;

	std::vector<std::decay_t<decltype(*p)>> L(n1);
	std::vector<std::decay_t<decltype(*p)>> R(n2);

	for (i = 0; i < n1; i++)
	{
		L[i] = *(p + i);
	}

	for (j = 0; j < n2; j++)
	{
		R[j] = *(q + j + 1);
	}
	i = 0, j = 0;

	for (k = p; i < n1 && j < n2; k++)
	{
		if (L[i] < R[j])
		{
			*k = L[i++];
		}
		else
		{
			*k = R[j++];
		}
	}

	while (i < n1)
	{
		*(k++) = L[i++];
	}

	while (j < n2)
	{
		*(k++) = R[j++];
	}
}

template <typename iter_t>
void merge_sort(iter_t p, iter_t r)
{
	int q;
	if (p < r)
	{
		q = (r - p) / 2;
		merge_sort(p, p + q);
		merge_sort(p + q + 1, r);
		merge(p, p + q, r);
	}
}

template <typename iter_t, typename T>
size_t search(iter_t start, iter_t end, T value) {
	for (iter_t begin = start; begin != end; ++begin)
	{
		if (*begin == value)
		{
			return begin - start;
		}
	}
	return end - start + 1;
}

template<>
size_t search<double*, double>(double* start, double* end, double value) {
	double epsilon = 0.0001;
	for (double* begin = start; begin != end; ++begin)
	{
		if (std::abs(*begin - value) < epsilon)
		{
			return begin - start;
		}
	}
	return end - start + 1;
}

template <typename T, size_t N>
size_t array_size(const T(&)[N]) {
	return N;
}

template <typename ValueT, typename KeyT=int>
class KeyValue
{
public:
	KeyValue()
	{}

	void append(KeyT key, ValueT value) {
		values.push_back(value);
		keys.push_back(key);
	}

	ValueT get(KeyT key) {
		this->sort();
		return values[bin_search(key)];
	}

private:
	void sort() {
		std::vector<KeyT> old_keys = keys;
		std::vector<ValueT> new_values(values.size());
		merge_sort(&keys[0], &keys[0 + keys.size() - 1]);

		for (int i = 0; i < new_values.size(); ++i) {
			new_values[i] = values[search(&old_keys[0], &old_keys[0 + old_keys.size()], keys[i])];
		}
		values = new_values;
	}

	template<typename KeyType>
	size_t bin_search(KeyType key) {
//		BinSearchError err;
		size_t begin = keys.size() / 2;
		while (0 <= begin <= keys.size() - 1)
		{
			if (keys[begin] > key)
			{
				begin = begin / 2;
			}
			else if (keys[begin] < key)
			{
				begin = (begin + keys.size()) / 2;
			}
			else if (keys[begin] == key)
			{
				return begin;
			}
		}
		if (keys[0] == key)
		{
			return 0;
		}
		if (keys[keys.size() - 1] == key)
		{
			return keys.size() - 1;
		}
//		throw(err);
	}

	template<>
	size_t bin_search<double>(double key) {
//		BinSearchError err;
		size_t begin = keys.size() / 2;
		while (0 <= begin <= keys.size() - 1)
		{
			if (abs(keys[begin] - key) <= epsilon)
			{
				return begin;
			}
			else if (keys[begin] < key)
			{
				begin = (begin + keys.size()) / 2;
			}
			else if (keys[begin] > key)
			{
				begin = begin / 2;
			}
		}
		if (abs(keys[0] - key) <= epsilon)
		{
			return 0;
		}
		if (abs(keys[keys.size() - 1] - key) <= epsilon)
		{
			return keys.size() - 1;
		}
//		throw(err);
	}
	

	std::vector<ValueT> values;
	std::vector<KeyT> keys;
	double epsilon = 0.0001;
};

class Error {
public:
	void virtual perr() = 0;
};

class SearchError : public Error {
public:
	void perr() override {
		std::cout << "Something wrong with search!";
	}
};

class MergeError : public Error {
public:
	void perr() override {
		std::cout << "Something wrong with merge_sort!";
	}
};

class KeyValueError : public Error {
public:
	void perr() override {
		std::cout << "Something wrong with KeyValue class!";
	}
};

class BinSearchError : public Error {
public:
	void perr() override {
		std::cout << "No such key was found!";
	}
};

void search_test() {
	bool flag = true;
	SearchError err;

	int a[] = { 1, 2, 3 };
	if (search(a, a + array_size(a), 3) != 2) flag = false;

	char b[] = "hello";
	if (search(b, b + array_size(b), 'g') != 7) flag = false;

	double c[] = { 1.2, 1.776 };
	if (search(c, c + array_size(c), 1.776) != 1) flag = false;

	std::vector<double> d = { 1.567, 1312.957891, 123.87 };
	if (search(&d[0], &d[0 + d.size()], 1312.957892) != 1) flag = false;

	if (!flag)
	{
		throw(err);
	}
}

void merge_test() {
	bool flag = true;
	MergeError err;

	std::vector<double> d = { 1.567, 1312.957891, 123.87 };
	merge_sort(&d[0], &d[0 + d.size() - 1]);
	if (d[0] != 1.567 or d[1] != 123.87 or d[2] != 1312.957891) flag = false;

	char b[] = "hello";
	merge_sort(&b[0], &b[0 + array_size(b) - 1]);
	if (b[1] != 'e' or b[2] != 'h' or b[3] != 'l' or b[5] != 'o') flag = false;

	if (!flag)
	{
		throw(err);
	}
}

void KeyValue_test() {
	bool flag = true;
	KeyValueError err;

	KeyValue<int> obj;

	obj.append(7, 89);
	obj.append(1, 97);
	obj.append(5, 12);

	if (obj.get(7) != 89 or obj.get(5) != 12 or obj.get(1) != 97) flag = false;

	KeyValue<bool> obj1;

	obj1.append(7, false);
	obj1.append(1, true);
	obj1.append(5, false);

	if (obj1.get(7) != false or obj1.get(5) != false or obj1.get(1) != true) flag = false;

	KeyValue<std::string> obj2;

	obj2.append(7, "Hello");
	obj2.append(1, "Where");
	obj2.append(5, "Who?");

	if (obj2.get(7) != "Hello" or obj2.get(5) != "Who?" or obj2.get(1) != "Where") flag = false;


	if (!flag)
	{
		throw(err);
	}

}

int main()
{
	try {
		search_test();
		merge_test();
	}
	catch (SearchError err) {
		err.perr();
		return 1;
	}
	catch (MergeError err) {
		err.perr();
		return 1;
	}
	catch (KeyValueError err) {
		err.perr();
		return 1;
	}
	catch (BinSearchError err) {
		err.perr();
	}
	catch (...) {
		std::cout << "Something unexpected happened!";
	}
}