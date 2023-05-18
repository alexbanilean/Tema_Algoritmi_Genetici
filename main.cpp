#include <bits/stdc++.h>
#include <random>

using namespace std;

ifstream f(R"(D:\Proiecte\CLion\AA\input.txt)");
ofstream g(R"(D:\Proiecte\CLion\AA\evolutie.txt)");

int dimensiune_populatie, parametru_a, parametru_b, parametru_c, precizie, numar_de_etape, lungime_cromozom, etapa_curenta;
double domeniu_definitie_x, domeniu_definitie_y, probabilitate_recombinare, probabilitate_mutatie, pas_discretizare;
vector<double> probabilitati_selectie, intervale_probabilitati_selectie;
vector<string> populatie;

void read() {

//    cout << "Introduceti datele in urmatorul format:\n"
//         << "Dimensiunea populatiei (numarul de cromozomi)\n"
//         << "Domeniul de definitie al functiei (capetele unui interval inchis)\n"
//         << "Parametrii pentru functia de maximizat (coeficientii polinomului de grad 2)\n"
//         << "Precizia cu care se lucreaza (cu care se discretizeaza intervalul)\n"
//         << "Probabilitatea de recombinare (crossover, incrucisare)\n"
//         << "Probabilitatea de mutatie\n"
//         << "Numarul de etape al algoritmului\n";

    f >> dimensiune_populatie;
    f >> domeniu_definitie_x >> domeniu_definitie_y;
    f >> parametru_a >> parametru_b >> parametru_c;
    f >> precizie;
    f >> probabilitate_recombinare;
    f >> probabilitate_mutatie;
    f >> numar_de_etape;

}

bool random_bit() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> distribution(0, 1);

    return distribution(gen);
}

vector<string> genereaza_populatie_random() {
    vector<string> tmp;

    for(int i = 0; i < dimensiune_populatie; ++i) {
        string s(lungime_cromozom, '0');
        for(int j = 0; j < lungime_cromozom; ++j) {
            if(random_bit())
                s[j] = '1';
        }

        tmp.push_back(s);
    }

    return tmp;
}

int get_10_exp(int p) {
    int sol = 1;

    while(p--)
        sol *= 10;

    return sol;
}

int get_val(string s, int l) {
    int p = 1, sol = 0;

    --l;
    while(l > -1) {
        if(s[l] == '1')
            sol += p;

        p *= 2;
        l--;
    }

    return sol;
}

double functie_fitness(double x) {

    if(parametru_a * x * x + parametru_b * x + parametru_c < 0) {
        cout << "Eroare valoare negativa!\n";
        return 0;
//        return abs(parametru_a * x * x + parametru_b * x + parametru_c);
    }

    return parametru_a * x * x + parametru_b * x + parametru_c;
}

vector<double> calculeaza_probabilitati_selectie(vector<string>& pop) {
    vector<double> valori, tmp;
    double F = 0;

    for(auto &c : pop) {
        double c_val = functie_fitness( domeniu_definitie_x + get_val(c, lungime_cromozom) * pas_discretizare);
        valori.push_back(c_val);
        F += c_val;
    }

    for(auto &c_val : valori) {
        tmp.push_back(c_val / F);
    }

    return tmp;
}

vector<double> calculeaza_intervale_probabilitati_selectie(vector<double>& prob) {
    vector<double> tmp;
    double idx = 0;

    for(auto &c_prob : prob) {
        tmp.push_back(idx);
        idx += c_prob;
    }

    tmp.push_back(idx);

    return tmp;
}

double random_u() {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> distribution(0, 1);

    return distribution(gen);
}

int cauta_binar_indice(double u, vector<double>& interv) {
    int st = 1, dr = dimensiune_populatie - 1;

    while(st <= dr) {
        int m = (st + dr) / 2;

        if(interv[m - 1] <= u && u <= interv[m + 1])
            return m;
        else if(u < interv[m])
            dr = m - 1;
        else
            st = m + 1;
    }

    return st - 1;
}

vector<string> metoda_ruletei(vector<string>& pop, vector<double>& interv) {
    vector<string> populatie_noua;

    for(int i = 0; i < dimensiune_populatie - 1; ++i) {
        double u = random_u();

        int c_id = cauta_binar_indice(u, interv);

        if(etapa_curenta == 0) {
            g << "u= " << u << " selectam cromozomul " << c_id << '\n';
        }

        populatie_noua.push_back(pop[c_id]);
    }

    return populatie_noua;
}

string elita(vector<string>& pop) {
    string best = pop[0];
    double max = INT_MIN;

    for(auto &c : pop) {
        double c_val = functie_fitness(domeniu_definitie_x + get_val(c, lungime_cromozom) * pas_discretizare);
        if(c_val > max) {
            max = c_val;
            best = c;
        }
    }

    return best;
}

vector<int> calculeaza_populatie_incrucisare(vector<string>& pop) {
    vector<int> tmp;

    if(etapa_curenta == 0) {
        g << "\nProbabilitatea de incrucisare " << probabilitate_recombinare << '\n';
    }

    for(int i = 0; i < dimensiune_populatie; ++i) {
        double c_p = random_u();

        if(c_p < probabilitate_recombinare) {
            tmp.push_back(i);
        }

        if(etapa_curenta == 0) {
            g << (i < 9 ? " " : "") << i + 1 << ": " << pop[i] << " u=";
            g << fixed << setprecision(precizie + 1) << c_p;
            if(c_p < probabilitate_recombinare) {
                g << " < " << probabilitate_recombinare << " participa\n";
            }
            else {
                g << '\n';
            }
        }
    }

    if(etapa_curenta == 0) {
        g << '\n';
    }

    return tmp;
}

pair<string, string> incruciseaza_cromozomi(string& c1, string& c2, int p_r) {
    if(etapa_curenta == 0) {
        g << c1 << " " << c2 << " punct " << p_r << '\n';
    }

    string nc1 = c1;
    string nc2 = c2;

    for(int i = 0; i < p_r; ++i) {
        nc1[i] = c2[i];
        nc2[i] = c1[i];
    }

    if(etapa_curenta == 0) {
        g << "Rezultat " << nc1 << " " << nc2 << '\n';
    }

    return {nc1, nc2};
}

int random_point(int l) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> distribution(0, l);

    return distribution(gen);
}

vector<string> incruciseaza(vector<string>& pop) {
    vector<int> candidati = calculeaza_populatie_incrucisare(pop);
    vector<string> tmp = pop;

    if(candidati.size() == 1)
        return pop;

    if(candidati.size() & 1) {
        int idx1 = candidati.back();
        candidati.pop_back();
        int idx2 = candidati.back();
        candidati.pop_back();
        int idx3 = candidati.back();
        candidati.pop_back();

        string c1 = pop[idx1], c2 = pop[idx2], c3 = pop[idx3];
        int punct_rupere = random_point(lungime_cromozom);

        if(etapa_curenta == 0) {
            g << "Recombinare dintre cromozomul " << idx1 << " cu cromozomul " << idx2 << ":\n";
        }

        auto c_copii_1 = incruciseaza_cromozomi(c1, c2, punct_rupere);

        if(etapa_curenta == 0) {
            g << "Recombinare dintre cromozomul " << idx2 << " cu cromozomul " << idx3 << ":\n";
        }

        auto c_copii_2 = incruciseaza_cromozomi(c2, c3, punct_rupere);

        tmp[idx1] = c_copii_1.first;
        tmp[idx2] = c_copii_1.second;
        tmp[idx3] = c_copii_2.second;
    }

    while(!candidati.empty()) {
        int idx1 = candidati.back();
        candidati.pop_back();
        int idx2 = candidati.back();
        candidati.pop_back();

        string c1 = pop[idx1], c2 = pop[idx2];
        int punct_rupere = random_point(lungime_cromozom);

        if(etapa_curenta == 0) {
            g << "Recombinare dintre cromozomul " << idx1 << " cu cromozomul " << idx2 << ":\n";
        }

        auto c_copii = incruciseaza_cromozomi(c1, c2, punct_rupere);

        tmp[idx1] = c_copii.first;
        tmp[idx2] = c_copii.second;
    }

    return tmp;
}

vector<int> calculeaza_populatie_mutatie() {
    vector<int> tmp;

    if(etapa_curenta == 0) {
        g << "\nProbabilitatea de mutatie pentru fiecare gena " << probabilitate_mutatie << '\n';
        g << "Au fost modificati cromozomii:\n";
    }

    for(int i = 0; i < dimensiune_populatie; ++i) {
        double c_p = random_u();

        if(c_p < probabilitate_mutatie) {
            tmp.push_back(i);

            if(etapa_curenta == 0) {
                g << i + 1 << '\n';
            }
        }
    }

    if(etapa_curenta == 0) {
        g << '\n';
    }

    return tmp;
}

vector<string> mutatie(vector<string>& pop) {
    vector<int> candidati = calculeaza_populatie_mutatie();
    vector<string> tmp = pop;

    for(auto &i: candidati) {
        int punct_mutatie = random_point(lungime_cromozom);

        tmp[i][punct_mutatie] = tmp[i][punct_mutatie] == '1' ? '0' : '1';
    }

    return tmp;
}

void afiseaza_populatie(vector<string>& pop) {
    for(int i = 0; i < dimensiune_populatie; ++i) {
        double tmp = domeniu_definitie_x + get_val(pop[i], lungime_cromozom) * pas_discretizare;
        g << (i < 9 ? " " : "") << i + 1 << ": " << pop[i] << " x= "<< (tmp < 0 ? "" : " ");
        g << fixed << setprecision(precizie + 1) << tmp << " f= ";
        g << fixed << setprecision(precizie + 1) << functie_fitness(tmp);
        g << '\n';
    }
}

double calculeaza_fitness_mediu(vector<string>& pop) {
    double avg = 0;

    for(auto &c : pop) {
        double c_val = functie_fitness( domeniu_definitie_x + get_val(c, lungime_cromozom) * pas_discretizare);
        avg += c_val;
    }

    return avg / dimensiune_populatie;
}

double calculeaza_fitness_maxim(vector<string>& pop) {
    double best = INT_MIN;

    for(auto &c : pop) {
        double c_val = functie_fitness( domeniu_definitie_x + get_val(c, lungime_cromozom) * pas_discretizare);
        if(c_val > best) {
            best = c_val;
        }
    }

    return best;
}

int main() {

    read();

    lungime_cromozom = ceil(log2((domeniu_definitie_y - domeniu_definitie_x) * get_10_exp(precizie)));
    pas_discretizare = (domeniu_definitie_y - domeniu_definitie_x) / ((1 << lungime_cromozom) - 1);

    g << "Evolutie\n";

    /// Pas 1: generare aleatoare pentru prima populatie
    populatie = genereaza_populatie_random();

    g << "\nPopulatie initiala\n";
    afiseaza_populatie(populatie);

    while(etapa_curenta < numar_de_etape) {
        /// Pas 2: calculare probabilitati de selectie pentru fiecare cromozom
        probabilitati_selectie = calculeaza_probabilitati_selectie(populatie);

        /// Pas 3: calculare intervale probabilitati de selectie
        intervale_probabilitati_selectie = calculeaza_intervale_probabilitati_selectie(probabilitati_selectie);

        if(etapa_curenta == 0) {
            g << "\nProbabilitati selectie\n";
            for(int i = 0; i < dimensiune_populatie; ++i) {
                g << "cromozom " << (i < 9 ? " " : "") << i + 1;
                g << " probabilitate " << fixed << setprecision(precizie + 1) << probabilitati_selectie[i] << '\n';
            }

            g << "\nIntervale probabilitati selectie\n";
            for(int i = 0; i <= dimensiune_populatie; ++i) {
                g << fixed << setprecision(precizie + 1) << intervale_probabilitati_selectie[i] << ' ';
            }
            g << '\n';
        }

        /// Pas 4: aplicare metoda ruletei (selectare n - 1 indivizi)
        vector<string> populatie_noua = std::move(metoda_ruletei(populatie, intervale_probabilitati_selectie));

        /// Pas 5: selectie elita
        string elite = elita(populatie);
        populatie_noua.push_back(elite);

        if(etapa_curenta == 0) {
            g << "\nSelectie elitista\n" << "Cel mai bun cromozom: " << elite << '\n';

            g << "\nDupa selectie\n";
            afiseaza_populatie(populatie_noua);
        }

        /// Pas 6: selectie populatie incrucisare + incrucisare
        populatie_noua = std::move(incruciseaza(populatie_noua));

        if(etapa_curenta == 0) {
            g << "\nDupa recombinare\n";
            afiseaza_populatie(populatie_noua);
        }

        /// Pas 7: selectie populatie mutatie + mutatie
        populatie_noua = std::move(mutatie(populatie_noua));

        if(etapa_curenta == 0) {
            g << "\nDupa mutatie\n";
            afiseaza_populatie(populatie_noua);

            g << "\nEvolutia valorii medii si valorii maxime a fitness-ului\n";
        }

        g << "avg = " << fixed << setprecision(2 * precizie) << calculeaza_fitness_mediu(populatie_noua);
        g << " max = " << fixed << setprecision(2 * precizie) << calculeaza_fitness_maxim(populatie_noua) << '\n';

        populatie = std::move(populatie_noua);
        ++etapa_curenta;
    }

    return 0;
}
