#include "signal_utils.h"
#include <algorithm>

double percentile (dlib::matrix<double> signal, double percentile) {
	std::sort(signal.begin(), signal.end());
	double result = signal(percentile * signal.nr(), 0);
	return result;
}

void fill_with_last(dlib::matrix<double>& signal, double to_replace) {
	double last_correct_value = to_replace;

	for (int i = 0; i != signal.nr(); ++i) {
		if (signal(i, 0) == to_replace || signal(i, 0) != signal(i, 0)) {
			signal(i, 0) = last_correct_value;
		} else {
			last_correct_value = signal(i, 0);
		}
	}
}

/*
 * dlib's implementation of standard deviation computes the unbiased estimator of
 * standard deviation: std(x) = 1/(n-1) * sum( (x_i - x_mean)^2 )
 *
 * here we want to have the usual estimation so that the results are consistent
 * with the onse from scikit-learn's StandardScaler, i.e.: std(x) = 1/n * sum( (x_i - x_mean)^2 )
 */
double standard_deviation(const dlib::matrix<double> &signal) {
	double mean = dlib::mean(signal);
	dlib::matrix<double> deviations =  (signal - mean);
	deviations = dlib::pointwise_multiply(deviations, deviations);

	double variance = dlib::mean(deviations);
	double stddev = sqrt(variance);
	return stddev;
}


dlib::matrix<int> nonnan_rows(const dlib::matrix<double>& input) {
	std::vector<int> v;

	for (std::size_t i = 0; i != input.nr(); ++i) {
		bool finite = dlib::is_finite(dlib::rowm(input, i));
		if (finite) {
			v.push_back(i);
		}
	}

	dlib::matrix<int> result(v.size(), 1);
	for (std::size_t i = 0; i != v.size(); ++i) {
		result(i,0) = v[i];
	}
	return result;
}

double nan_ratio(const dlib::matrix<double>& input) {
	int nans = 0;
	for (std::size_t i = 0; i != input.nr(); ++i) {
		bool finite = dlib::is_finite(dlib::rowm(input, i));
		if (!finite) {
			nans++;
		}
	}

	return double(nans) / input.nr();
}


dlib::matrix<int> rows_greater_than(const dlib::matrix<double> &signal, double threshold) {
	std::vector<int> rows;

	for (int i = 0; i != signal.nr(); ++i) {
		double value = signal(i, 0);
		if (value > threshold) {
			rows.push_back(i);
		}
	}

	dlib::matrix<int> result(rows.size(), 1);
	for (std::size_t i = 0; i != rows.size(); ++i) {
		result(i, 0) = rows[i];
	}

	return result;
}


dlib::matrix<double> normalize(const dlib::matrix<double> &signal) {
	//TODO: make sure it works with negative values also
	double sum = dlib::sum(signal);
	return signal / sum;
}

double entropy(const dlib::matrix<double> &signal) {
	dlib::matrix<double> normalized = normalize(signal);
	dlib::matrix<double> to_sum = dlib::pointwise_multiply(normalized, dlib::log(normalized));
	double result = (-1) * dlib::sum(to_sum);
	return result;
}
