#ifndef _replay_hpp
#define _replay_hpp
#include <Windows.h>
#include <vector>
#include <fstream>

class Replay {
public:
    std::map<int, std::vector<bool>> p1_inputs;
    std::map<int, std::vector<bool>> p2_inputs;
    float delta;
    std::string name;
    float version;

    void save() {
        std::ofstream myfile;

        if (CreateDirectory("replays", NULL) ||
            ERROR_ALREADY_EXISTS == GetLastError())
        {
            myfile.open("replays\\" + name + ".zbf", std::ios::binary);

            myfile.write(reinterpret_cast<const char*>(&delta), sizeof(float));
            myfile.write(reinterpret_cast<const char*>(&version), sizeof(float));
            
            for (auto it = p1_inputs.begin(); it != p1_inputs.end(); it++) {
                for (auto el : it->second) {
                    myfile.write(reinterpret_cast<const char*>(&it->first), sizeof(int));
                    myfile << el << true;
                }
            }
            
            for (auto it = p2_inputs.begin(); it != p2_inputs.end(); it++) {
                for (auto el : it->second) {
                    myfile.write(reinterpret_cast<const char*>(&it->first), sizeof(int));
                    myfile << el << false;
                }
            }

            myfile.close();
        }
    }

    static Replay* fromFile(std::string filename) {
        if (CreateDirectory("replays", NULL) ||
            ERROR_ALREADY_EXISTS == GetLastError())
        {
            std::ifstream infile("replays\\" + filename + ".zbf", std::ios::binary);
            if (!infile.is_open())
            {
                return nullptr;
            }

            Replay* rec = new Replay();

            char* dR = new char[sizeof(float) + 1];

            infile.read(dR, sizeof(float));

            rec->delta = *reinterpret_cast<float*>(dR);

            infile.read(dR, sizeof(float));

            rec->version = *reinterpret_cast<float*>(dR);

            while (infile.peek() != EOF) {
                infile.read(dR, sizeof(float));
                int frame = *reinterpret_cast<int*>(dR);

                char str[3];

                infile.read(str, 2);
                bool click = str[0] != '0';

                bool p1 = str[1] != '0';

                if (p1) {
                    rec->p1_inputs[frame].push_back(click);
                } else {
                    rec->p2_inputs[frame].push_back(click);
                }
            }


            infile.close();

            auto pos = filename.rfind(".zbf");
            if (pos != std::string::npos) {
                filename.erase(pos);
            }

            rec->name = filename;

            return rec;
        }
        return nullptr;
    }

    void purgeInputs(int frame) {
        p1_inputs.erase(p1_inputs.find(frame), p1_inputs.end());
        p2_inputs.erase(p2_inputs.find(frame), p2_inputs.end());
    }
};

#endif