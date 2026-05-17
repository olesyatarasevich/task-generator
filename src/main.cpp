#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iomanip>

#ifdef _WIN32
    #include <direct.h>
    #define MKDIR(path) _mkdir(path)
#else
    #include <sys/stat.h>
    #define MKDIR(path) mkdir(path, 0755)
#endif

void float_to_binary(float f, bool binary[32]) {
    if (f == 0.0f) { for(int i=0;i<32;i++) binary[i]=0; return; }
    binary[0] = f < 0; if (f < 0) f = -f;
    unsigned long long int_part = (unsigned long long)f;
    float frac_part = f - int_part;
    bool int_bin[23]; for (int i=22;i>=0;i--) { int_bin[i]=int_part%2; int_part/=2; }
    bool frac_bin[23]; for (int i=0;i<23;i++) { frac_part*=2; frac_bin[i]=(frac_part<1)?0:1; if(frac_bin[i]) frac_part-=1; }
    bool f_bin[46]; for(int i=0;i<23;i++) f_bin[i]=int_bin[i]; for(int i=23;i<46;i++) f_bin[i]=frac_bin[i-23];
    int shift=0, idx_man=0;
    for(int i=0;i<46;i++) if(f_bin[i]) { shift=22-i; idx_man=i+1; break; }
    int exp=127+shift; for(int i=8;i>0;i--) { binary[i]=exp%2; exp/=2; }
    for(int i=9;i<32;i++) binary[i]=(idx_man<46)?f_bin[idx_man++]:0;
}

void double_to_binary(double d, bool binary[64]) {
    if (d == 0.0) { for(int i=0;i<64;i++) binary[i]=0; return; }
    binary[0] = d < 0; if (d < 0) d = -d;
    unsigned long long int_part = (unsigned long long)d;
    double frac_part = d - int_part;
    bool int_bin[52]; for (int i=51;i>=0;i--) { int_bin[i]=int_part%2; int_part/=2; }
    bool frac_bin[52]; for (int i=0;i<52;i++) { frac_part*=2; frac_bin[i]=(frac_part<1)?0:1; if(frac_bin[i]) frac_part-=1; }
    bool d_bin[104]; for(int i=0;i<52;i++) d_bin[i]=int_bin[i]; for(int i=52;i<104;i++) d_bin[i]=frac_bin[i-52];
    int shift=0, idx_man=0;
    for(int i=0;i<104;i++) if(d_bin[i]) { shift=51-i; idx_man=i+1; break; }
    int exp=1023+shift; for(int i=11;i>0;i--) { binary[i]=exp%2; exp/=2; }
    for(int i=12;i<64;i++) binary[i]=(idx_man<104)?d_bin[idx_man++]:0;
}

void binary_to_float(const bool binary[32], float *result) {
    bool all_zero=true; for(int i=1;i<32;i++) if(binary[i]) all_zero=false;
    if(all_zero && !binary[0]) { *result=0.0f; return; }
    int sign=binary[0]?-1:1, exp=-127, w=128;
    for(int i=1;i<=8;i++) { if(binary[i]) exp+=w; w/=2; }
    float m=1.0, p=0.5; for(int i=9;i<32;i++) { if(binary[i]) m+=p; p*=0.5; }
    if(exp>0) for(int i=0;i<exp;i++) m*=2.0; else for(int i=0;i<-exp;i++) m*=0.5;
    *result = sign*m;
}

void binary_to_double(const bool binary[64], double *result) {
    bool all_zero=true; for(int i=1;i<64;i++) if(binary[i]) all_zero=false;
    if(all_zero && !binary[0]) { *result=0.0; return; }
    int sign=binary[0]?-1:1, exp=-1023, w=1024;
    for(int i=1;i<=11;i++) { if(binary[i]) exp+=w; w/=2; }
    double m=1.0, p=0.5; for(int i=12;i<64;i++) { if(binary[i]) m+=p; p*=0.5; }
    if(exp>0) for(int i=0;i<exp;i++) m*=2.0; else for(int i=0;i<-exp;i++) m*=0.5;
    *result = sign*m;
}

std::string toBinStr(const bool* arr, int len) {
    std::string s;
    for (int i=0; i<len; i++) { s += arr[i] ? '1' : '0'; if ((i+1)%8==0 && i!=len-1) s += ' '; }
    return s;
}

double getRandom(double a, double b, int p) {
    double val = a + (double)rand()/RAND_MAX * (b-a);
    double mul = pow(10.0, p);
    return round(val*mul)/mul;
}

int main() {
    std::ifstream cfg("config.txt");
    if (!cfg.is_open()) { std::cerr << "Не найден файл cofig.txt\n"; return 1; }
    int n, k, bits; double a, b; int p;
    cfg >> n >> k >> bits >> a >> b >> p;
    cfg.close();
    MKDIR("../Задания"); MKDIR("../Проверка");
    srand(time(NULL));
    for (int v=1; v<=n; v++) {
        std::ofstream tOut("../Задания/Вариант_" + std::to_string(v) + ".md");
        std::ofstream cOut("../Проверка/Вариант_" + std::to_string(v) + ".md");
        tOut << "# Вариант " << v << "\n\n| № | Число |\n|---|---|\n";
        cOut << "# Вариант " << v << "\n\n| № | Число | Машинное представление | Точность |\n|---|---|---|---|\n";
        for (int i=0; i<k; i++) {
            double num = getRandom(a, b, p);
            tOut << "| " << i+1 << " | " << num << " |\n";
            cOut << "| " << i+1 << " | " << num << " | ... | ... |\n";
        }
    }
    return 0;
}
