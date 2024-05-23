#include <iostream>
#include <vector>
#include <string>
#include <bitset>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <set>

using namespace std;

vector<string> solution(vector<int> inputs){
    int num_vars=inputs[0];
    vector<int> minterms(inputs.begin()+2, inputs.end());

    auto toBinaryString=[&](int num){
        return bitset<16>(num).to_string().substr(16-num_vars, num_vars);
    };

    auto combineTerms=[&](const string& term1,const string& term2){
        string combined=term1;
        int diff_count=0;
        for(size_t cnt_i=0; cnt_i<term1.size(); ++cnt_i){
            if(term1[cnt_i]!=term2[cnt_i]){
                combined[cnt_i]='-';
                ++diff_count;
            }
        }
        return diff_count==1 ? combined : "";
    };

    vector<vector<string>> groups(num_vars+1);
    unordered_map<string,bool> is_combined;

    for(int minterm : minterms){
        string binary=toBinaryString(minterm);
        int ones_count=count(binary.begin(), binary.end(), '1');
        groups[ones_count].push_back(binary);
        is_combined[binary]=false;
    }

    unordered_set<string> prime_implicants;
    while(true) {
        vector<vector<string>> new_groups(num_vars+1);
        bool found_combination=false;

        for(int cnt_i=0; cnt_i<num_vars; ++cnt_i){
            for(const string& term1 : groups[cnt_i]){
                for(const string& term2 : groups[cnt_i+1]){
                    string combined=combineTerms(term1, term2);
                    if(!combined.empty()) {
                        new_groups[count(combined.begin(), combined.end(), '1')].push_back(combined);
                        is_combined[term1]=true;
                        is_combined[term2]=true;
                        is_combined[combined]=false;
                        found_combination=true;
                    }
                }
            }
        }

        for(const auto& group : groups){
            for(const auto& term : group){
                if(!is_combined[term]){
                    prime_implicants.insert(term);
                }
            }
        }

        if(!found_combination) break;
        groups=new_groups;
    }

    for(const auto& group : groups){
        for(const auto& term : group){
            if(!is_combined[term]){
                prime_implicants.insert(term);
            }
        }
    }

    vector<string> result(prime_implicants.begin(), prime_implicants.end());

    auto customCompare=[](const string& a,const string& b){
        for(size_t cnt_i=0; cnt_i<a.size(); ++cnt_i) {
            char charA=(a[cnt_i]=='-') ? '2' : a[cnt_i];
            char charB=(b[cnt_i]=='-') ? '2' : b[cnt_i];
            if(charA!=charB) {
                return charA<charB;
            }
        }
        return false;
    };
    sort(result.begin(), result.end(), customCompare);

    unordered_map<int,vector<string>> mintermCoverage;
    for(const string& pi : result){
        for(int minterm : minterms){
            string binary=toBinaryString(minterm);
            bool covers=true;
            for(int cnt_i=0; cnt_i<num_vars; ++cnt_i){
                if(pi[cnt_i]!='-'&&pi[cnt_i]!=binary[cnt_i]) {
                    covers=false;
                    break;
                }
            }
            if(covers){
                mintermCoverage[minterm].push_back(pi);
            }
        }
    }

    set<string> essentialPrimeImplicants;
    for(const auto& entry : mintermCoverage){
        if(entry.second.size()==1){
            essentialPrimeImplicants.insert(entry.second[0]);
        }
    }

    vector<string> epi(essentialPrimeImplicants.begin(), essentialPrimeImplicants.end());
    sort(epi.begin(), epi.end(), customCompare);

    vector<string> final_result(result.begin(), result.end());
    final_result.push_back("EPI");
    if(!epi.empty()){
        final_result.insert(final_result.end(), epi.begin(), epi.end());
    }

    return final_result;
}

int main() {
    int num_vars, num_minterms;
    cin>>num_vars>>num_minterms;

    vector<int> inputs={num_vars, num_minterms};
    for(int cnt_i=0; cnt_i<num_minterms; ++cnt_i){
        int minterm;
        cin>>minterm;
        inputs.push_back(minterm);
    }

    vector<string> output=solution(inputs);

    for(const string& implicant : output){
        cout<<implicant<<" ";
    }

    return 0;
}