
#include <iostream>
#include <complex>
#include <vector>
#include <ctime>
#include <cmath>
#include <random>
#include <string>
#include <fstream>

using namespace std;

class gen {
    double real, imag;
    complex<double> result;

public:

    gen() {}

    gen(double& Ps) {
        real = sqrt(Ps / 2.);
        imag = sqrt(Ps / 2.);
        result.real(real);
        result.imag(imag);
        srand(time(0));
    }

    complex<double> next() {
        return (rand() % 2 == 0 ? result : -result);
    }

};

//double awgn(double& SNR, double& PsdB) {
//    default_random_engine engine;
//    normal_distribution<double> distr;
//    distr = normal_distribution<double>(0., PsdB - SNR);
//    return distr(engine);
//}

class awgn {
    default_random_engine generator;
    normal_distribution<double> distr;

public:

    awgn(const double& m, const double& s)
    {
        distr = normal_distribution<double>(m, 0.5 * pow(10., s / 10.)); //change
    }

    double next() {
        return distr(generator);
    }

};

class decisionmaker {
    complex<double> result;
    double Ps;

public:

    decisionmaker() {}

    decisionmaker(double Ps) {
        this->Ps = Ps;
    }

    complex<double> decide(complex<double> signal) {
        if (signal.imag() >= -signal.real()) {
            result.real(sqrt(Ps / 2.));
            result.imag(result.real());
            return result;
        }
        else {
            result.real(-sqrt(Ps / 2.));
            result.imag(result.real());
            return result;
        }
    }

};


int main()
{
    double Ps = 2.;
    gen generator(Ps);
    ofstream signalout("out1.txt");
    ofstream signalnoisedout("out1n.txt");
    ofstream signaldecisionmade("out1d.txt");
    ofstream signalberratio("out2.txt");
    complex<double> signal;
    complex<double> signaln;
    complex<double> signald;
    double ampsum = 0;
    double correct = 0;
    double incorrect = 0;
    double PsdB = 10. * log10(Ps);
    double SNR = 0;
    double PndB = PsdB - SNR;
    double BER_t;
    double N = 1000000;
    //awgn noise(0, PsdB - SNR);
    decisionmaker decision(Ps);
    int it = 0;
    for (int snr = 0; snr <= 10; ++snr) {
        awgn noise(0, PsdB - snr);
        PndB = PsdB - snr;
        ampsum = 0;
        correct = 0;
        incorrect = 0;
        for (int i = 0; i < N; ++i) {
            signal = generator.next();
            signaln = signal;
            signaln.real(signaln.real() + noise.next());
            signaln.imag(signaln.imag() + noise.next());
            signald = decision.decide(signaln);
            if (it == 0) {
                signalout << i << "\t" << signal.real() << (signal.imag() < 0 ? "" : "+") << signal.imag() << "i" << endl;
                signalnoisedout << i << "\t" << signaln.real() << (signaln.imag() < 0 ? "" : "+") << signaln.imag() << "i" << endl;
                signaldecisionmade << i << "\t" << signald.real() << (signald.imag() < 0 ? "" : "+") << signald.imag() << "i" << endl;
            }
            ampsum += pow(signald.real() - signaln.real(), 2) + pow(signald.imag() - signaln.imag(), 2);
            if (signald == signal)
                ++correct;
        }
        BER_t = 0.5 * erfc(sqrt(pow(10., snr / 10.)));
        signalberratio << snr << "\t" << (N - correct) / N << "\t" << BER_t << endl;
        ++it;
        cout << "Ps = " << Ps << " | Ps dB = " << PsdB << " | SNR dB = " << snr << endl;
        cout << "Pn = " << pow(10., PndB / 10.) << " | Pn dB = " << PndB << endl;
        cout << "Pn calculated = " << ampsum / N << " | dB = " << 10. * log10(ampsum / N) << endl;
        cout << "correct = " << correct << " | incorrect = " << N - correct << " | BER = " << (N - correct) / N << endl << endl;
    }

    signalout.close();
    signalnoisedout.close();
    signaldecisionmade.close();
    system("pause");
}
