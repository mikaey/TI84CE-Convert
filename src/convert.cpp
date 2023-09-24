#include <ti/screen.h>
#include <ti/getkey.h>
#include "convert.hpp"
// TODO: enforce compatible unit conversions (ie: can't convert g to m)

int main() {
    char input[64] = "";
    char outputUnit[32] = "";
    char output[128] = "";
    char exit[32] = "yes";
    while (exit[0] == 'y' || exit[0] == 'Y') {
        os_ClrHome();
        os_GetStringInput("Please enter the value to convert: ", input, 64);
        os_NewLine();
        os_GetStringInput("Please enter the unit to convert to: ", outputUnit, 32);
        convertFromString(input, outputUnit, output, 128);
        os_NewLine();
        os_PutStrFull("Result: ");
        os_PutStrFull(output);
        os_NewLine();
        os_GetStringInput("Would you like to convert something else? (Y)es or (N)o? ", exit, 32);
        os_NewLine();
        while (exit[0] != 'n' && exit[0] != 'N' && exit[0] != 'y' && exit[0] != 'Y') {
            os_PutStrFull("Please say \"(Y)es\" or \"(N)o.\" ");
            os_NewLine();
            os_GetStringInput("Would you like to convert something else? (Y)es or (N)o? ", exit, 32);
            os_NewLine();
        }
    }
}

/*string convert(long double rawValue, string unit, string convertedUnit) {
    map<string, double> prefix;
    prefix["Q"] = 1e30l;
    prefix["R"] = 1e27l;
    prefix["Y"] = 1e24l;
    prefix["Z"] = 1e21l;
    prefix["E"] = 1e18l;
    prefix["P"] = 1e15l;
    prefix["T"] = 1e12l;
    prefix["G"] = 1e9l;
    prefix["M"] = 1e6l;
    prefix["k"] = 1e3l;
    prefix["h"] = 1e2l;
    prefix["da"] = 10l;
    prefix[""] = 1l;
    prefix["d"] = 0.1l;
    prefix["c"] = 1e-2l;
    prefix["m"] = 1e-3l;
    prefix["μ"] = 1e-6l;
    prefix["u"] = 1e-6l;
    prefix["n"] = 1e-9l;
    prefix["p"] = 1e-12l;
    prefix["f"] = 1e-15l;
    prefix["a"] = 1e-18l;
    prefix["z"] = 1e-21l;
    prefix["y"] = 1e-24l;
    prefix["r"] = 1e-27l;
    prefix["q"] = 1e-30l;
    string unitPrefix = unit;
    string convertedUnitPrefix=convertedUnit;
    unitPrefix.pop_back();
    convertedUnitPrefix.pop_back();
    long double value = (rawValue * prefix[unitPrefix])/prefix[convertedUnitPrefix];
    return to_string(value) + convertedUnit;
} */