#include <HashMap-master/hashmap/HashMap.h>
#include <cstring>
#include <cstdio>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <cstdint>
#include <ti/real.h>
#include <ti/getcsc.h>
// TODO: enforce compatible unit conversions (ie: can't convert g to m)
struct unitConversion {
    real_t offset;
    real_t ratio;
};

template <size_t tableSize>
struct MyKeyHash {
    unsigned long operator()(const char* k) const{
        uint32_t hashAddress = 0;
        for (uint32_t counter = 0; counter < strlen(k); counter++) {
            hashAddress = k[counter] + (hashAddress << 6) + (hashAddress << 16) - hashAddress;
        }
        /* printf("String: %s ", k);
        printf("Hash: %x\n", hashAddress); */
        return hashAddress%tableSize;
    };
};

const char* unitList[] = {"LBS", "MIN", "MI", "HR", "M", "S", "MOL", "A", "K", "CD", "G", "V", "C", "FT", "YD", "IN", "F", "J", "N", "W", "L"};
const uint32_t numberOfUnits = 21;
const char* prefixList[] = {"R", "Y", "Z", "E", "P", "T", "G", "M", "K", "H", "DA", "", "D", "C", "M", "μ", "U", "N", "P", "F", "A"};
const uint32_t numberOfPrefixes = 21;

void convert(real_t rawValue, char* unitPrefix, char* unit, real_t unitExponent, char* convertedUnitPrefix, char* convertedUnit, real_t convertedExponent, char* output, int outputBufferLength) {
    HashMap<const char*, real_t, 50, MyKeyHash<50> > prefix;
    const real_t zero = os_Int24ToReal(0);
    const real_t one = os_Int24ToReal(1);
    const real_t five = os_Int24ToReal(5);
    const real_t nine = os_Int24ToReal(9);
    prefix.put("R", os_StrToReal("1e27", nullptr));
    prefix.put("Y", os_StrToReal("1e24", nullptr));
    prefix.put("Z", os_StrToReal("1e21", nullptr));
    prefix.put("E", os_StrToReal("1e18", nullptr));
    prefix.put("P", os_StrToReal("1e15", nullptr));
    prefix.put("T", os_StrToReal("1e12", nullptr));
    prefix.put("G", os_StrToReal("1e9", nullptr));
    prefix.put("M", os_StrToReal("1e6", nullptr));
    prefix.put("K", os_StrToReal("1e3", nullptr));
    prefix.put("H", os_StrToReal("1e2", nullptr));
    prefix.put("DA", os_StrToReal("10", nullptr));
    prefix.put("", one);
    prefix.put("D", os_StrToReal("0.1", nullptr));
    prefix.put("C", os_StrToReal("1e-2", nullptr));
    prefix.put("M", os_StrToReal("1e-3", nullptr));
    prefix.put("μ", os_StrToReal("1e-6", nullptr));
    prefix.put("U", os_StrToReal("1e-6", nullptr));
    prefix.put("N", os_StrToReal("1e-9", nullptr));
    prefix.put("P", os_StrToReal("1e-12", nullptr));
    prefix.put("F", os_StrToReal("1e-15", nullptr));
    prefix.put("A", os_StrToReal("1e-18", nullptr));
    // how to type lowercase letters
    /*prefix.put("z", os_FloatToReal(1e-21));
    prefix.put("y", os_FloatToReal(1e-24));
    prefix.put("r", os_FloatToReal(1e-27));*/
    const unitConversion baseUnit {zero, one};
    HashMap<char*, unitConversion, 50, MyKeyHash<50>> units;
    units.put("M", baseUnit);
    units.put("S", baseUnit);
    units.put("MOL", baseUnit);
    units.put("A", baseUnit);
    units.put("K", baseUnit);
    units.put("CD", baseUnit);
    units.put("G", baseUnit);
    units.put("V", baseUnit);
    units.put("C", unitConversion {os_StrToReal("273.15", nullptr), one});
    units.put("FT", unitConversion {zero, os_StrToReal("0.3048", nullptr)});
    units.put("YD", unitConversion {zero, os_StrToReal("0.9144", nullptr)});
    units.put("IN", unitConversion {zero, os_StrToReal("0.0254", nullptr)});
    units.put("MI", unitConversion {zero, os_StrToReal("1609.34", nullptr)});
    units.put("F", unitConversion {os_StrToReal("459.67", nullptr), os_RealDiv(&five, &nine)});
    units.put("J", baseUnit);
    units.put("N", baseUnit);
    units.put("W", baseUnit);
    units.put("L", unitConversion{zero, os_StrToReal("0.001", nullptr)});
    units.put("LBS", unitConversion{zero, os_StrToReal("0.4535924", nullptr)});
    units.put("MIN", unitConversion{zero, os_StrToReal("60", nullptr)});
    units.put("HR", unitConversion{zero, os_StrToReal("3600", nullptr)});

    real_t unitPrefixValue;
    real_t convertedUnitPrefixValue;
    unitConversion conversionForUnit;
    unitConversion conversionForConvertedUnit;
    bool unitPrefixGood = prefix.get(unitPrefix, unitPrefixValue);
    bool convertedUnitPrefixGood = prefix.get(convertedUnitPrefix, convertedUnitPrefixValue);
    bool unitGood = units.get(unit, conversionForUnit);
    bool convertedUnitGood = units.get(convertedUnit, conversionForConvertedUnit);
    /*if (!unitPrefixGood) {
        printf("Getting the unit prefix failed! Result: %Lf\n", unitPrefixValue);
    }
    if (!convertedUnitPrefixGood) {
        printf("Getting the output unit prefix failed! Result: %Lf\n", convertedUnitPrefixValue);
    }
    if (!unitGood) {
        printf("Getting the unit failed!\n");
    }
    if (!convertedUnitGood) {
        printf("Getting the output unit failed!\n");
    } */
    if (!(unitPrefixGood && convertedUnitPrefixGood && unitGood && convertedUnitGood)) {
        exit(1);
    }
    // rawValue + conversionForUnit.offset
    real_t temp1 = os_RealAdd(&rawValue, &conversionForUnit.offset);
    // unitPrefixValue*conversionForUnit.ratio
    real_t temp2 = os_RealMul(&unitPrefixValue, &conversionForUnit.ratio);
    // (unitPrefixValue*conversionForUnit.ratio)^unitExponent
    temp2 = os_RealPow(&temp2, &unitExponent);
    // (rawValue + conversionForUnit.offset)*powl(unitPrefixValue*conversionForUnit.ratio, unitExponent)
    temp1 = os_RealMul(&temp1, &temp2);
    // ((rawValue + conversionForUnit.offset)*powl(unitPrefixValue*conversionForUnit.ratio, unitExponent)-conversionForConvertedUnit.offset)
    temp1 = os_RealSub(&temp1, &conversionForConvertedUnit.offset);
    // convertedUnitPrefixValue*conversionForConvertedUnit.ratio
    temp2 = os_RealMul(&convertedUnitPrefixValue, &conversionForConvertedUnit.ratio);
    // (convertedUnitPrefixValue*conversionForConvertedUnit.ratio)^convertedExponent
    temp2 = os_RealPow(&temp2, &convertedExponent);
    // long double value = ((rawValue + conversionForUnit.offset)*powl(unitPrefixValue*conversionForUnit.ratio, unitExponent)-conversionForConvertedUnit.offset)/pow(convertedUnitPrefixValue*conversionForConvertedUnit.ratio, convertedExponent);
    real_t value = os_RealDiv(&temp1,&temp2);
    char convertedExponentString[20] = "";
    if (os_RealCompare(&convertedExponent, &one) != 0) {
        os_RealToStr(convertedExponentString, &convertedExponent, 20, 1, -1);
    }
    const int resultLength = strlen(convertedUnitPrefix) + strlen(convertedUnit) + strlen(convertedExponentString) + 20;
    char* result = new char[resultLength];
    os_RealToStr(result, &value, resultLength, 1, -1);

    // Debugging stuff
    /* printf("Unit Prefix: %Lf\n", unitPrefixValue);
    printf("Converted Unit Ratio: %Lf\n", conversionForConvertedUnit.ratio);
    printf("Final Prefix: %s\n", convertedUnitPrefix);
    printf("Final Unit: %s\n", convertedUnit); */
    strcat(result, convertedUnitPrefix);
    strcat(result, convertedUnit);
    strcat(result, convertedExponentString);
    // printf("Final Result: %s\n", result);
    strncpy(output, result, outputBufferLength);
    output[outputBufferLength - 1] = 0;
    delete[] result;
}

void convertFromString(const char* rawInput, const char* output, char* outputBuffer, int outputBufferLength) {
    real_t inputExponent = os_Int24ToReal(1);
    real_t outputExponent = os_Int24ToReal(1);
    char* input = new char[strlen(rawInput) + 1];
    char* outputUnit = new char[strlen(output) + 1];
    strcpy(input, rawInput);
    for (unsigned int i = 0; i < strlen(rawInput); i++) {
        input[i] = toupper(input[i]);
    }
    strcpy(outputUnit, output);
    for (unsigned int i = 0; i < strlen(outputUnit); i++) {
        outputUnit[i] = toupper(outputUnit[i]);
    }
    if (isdigit(input[strlen(input) - 1])) {
        inputExponent = os_StrToReal(&input[strlen(input) - 1], nullptr);
        input[strlen(input) - 1] = 0;
    }
    if (isdigit(outputUnit[strlen(outputUnit) - 1])) {
        outputExponent = os_StrToReal(&outputUnit[strlen(outputUnit) - 1], nullptr);
        outputUnit[strlen(outputUnit) - 1] = 0;
    }
    char* number = new char[strlen(input) + 1];
    unsigned int i = 0;
    while ((isdigit(input[i]) || input[i] == '.') && i < strlen(input)) {
        number[i] = input[i];
        i++;
    }
    char inputUnit[10] = "";
    i = 0;
    while (strcmp(inputUnit, "") == 0 && i < numberOfUnits) {
        if (strlen(input) >= strlen(unitList[i])) {
            char* substr = new char[strlen(unitList[i]) + 1];
            strncpy(substr, &input[strlen(input)-strlen(unitList[i])], strlen(unitList[i]));
            substr[strlen(unitList[i])] = 0;
            if (strcmp(substr, unitList[i]) == 0) {
                strcpy(inputUnit, unitList[i]);
                input[strlen(input)-strlen(unitList[i])] = 0;
            }
            delete[] substr;
        }
        i++;
    }
    char inputPrefix[10] = "";
    i = 0;
    while (strcmp(inputPrefix, "") == 0 && i < numberOfPrefixes) {
        if (strlen(input) >= strlen(prefixList[i])) {
            char* substr = new char[strlen(prefixList[i]) + 1];
            strncpy(substr, &input[strlen(input)-strlen(prefixList[i])], strlen(prefixList[i]));
            substr[strlen(prefixList[i])] = 0;
            if (strcmp(substr, prefixList[i]) == 0) {
                strcpy(inputPrefix, prefixList[i]);
                input[strlen(input)-strlen(prefixList[i])] = 0;
            }
            delete[] substr;
        }
        i++;
    }
    char outputUnitName[10] = "";
    i = 0;
    while (strcmp(outputUnitName, "") == 0 && i < numberOfUnits) {
        if (strlen(outputUnit) >= strlen(unitList[i])) {
            char* substr = new char[strlen(unitList[i]) + 1];
            strncpy(substr, &outputUnit[strlen(outputUnit)-strlen(unitList[i])], strlen(unitList[i]));
            substr[strlen(unitList[i])] = 0;
            if (strcmp(substr, unitList[i]) == 0) {
                strcpy(outputUnitName, unitList[i]);
                outputUnit[strlen(outputUnit)-strlen(unitList[i])] = 0;
            }
            delete[] substr;
        }
        i++;
    } 
    char outputPrefix[10] = "";
    i = 0;
    while (strcmp(outputPrefix, "") == 0 && i < numberOfPrefixes) {
        if (strlen(outputUnit) >= strlen(prefixList[i])) {
            char* substr = new char[strlen(prefixList[i]) + 1];
            strncpy(substr, &outputUnit[strlen(outputUnit)-strlen(prefixList[i])], strlen(prefixList[i]));
            substr[strlen(prefixList[i])] = 0;
            if (strcmp(substr, prefixList[i]) == 0) {
                strcpy(outputPrefix, prefixList[i]);
                outputUnit[strlen(outputUnit)-strlen(prefixList[i])] = 0;
            }
            delete[] substr;
        }
        i++;
    }
    if (strcmp(inputUnit, "") == 0) {
        os_NewLine();
        os_PutStrFull("Please put a unit for the source value. Press any key to exit.");
        while (!os_GetCSC());
        exit(1);
    }
    if (strcmp(outputUnitName, "") == 0) {
        os_NewLine();
        os_PutStrFull("Please put a unit to convert to. Press any key to exit.");
        while (!os_GetCSC());
        exit(1);
    }
    /* printf("Number: %s\n", number);
    printf("Input Prefix: %s\n", inputPrefix);
    printf("Input Unit: %s\n", inputUnit);
    printf("Input Exponent: %ld\n", inputExponent);
    printf("Output Prefix: %s\n", outputPrefix);
    printf("Output Unit: %s\n", outputUnitName);
    printf("Output Exponent: %ld\n", outputExponent); */
    convert(os_StrToReal(number, nullptr), inputPrefix, inputUnit, inputExponent, outputPrefix, outputUnitName, outputExponent, outputBuffer, outputBufferLength);
}