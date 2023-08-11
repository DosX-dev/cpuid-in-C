// Coded by DosX

#include <stdio.h>
#include <windows.h>

// Function to retrieve CPU information using CPUID instruction
void cpuid(int* info, int infoType) {
    __asm__(
        "cpuid"            // CPUID instruction: fetch processor information
        : "=a"(info[0]),   // Output: CPUID result (lower 32 bits) saved in info[0]
          "=b"(info[1]),   // Output: CPUID result (bits 32-63) saved in info[1]
          "=c"(info[2]),   // Output: CPUID result (bits 64-95) saved in info[2]
          "=d"(info[3])    // Output: CPUID result (bits 96-127) saved in info[3]
        : "a"(infoType));  // Input: infoType (value for CPUID) placed in register eax before CPUID execution
}

void main() {
    printf("Coded by DosX-dev (GitHub)\n\n");

    int info[4];

    // Get processor name (iterate through four sets of information)
    printf("Processor Name: ");

    for (int i = 2; i <= 4; ++i) {
        cpuid(info, 0x80000002 + i - 2);
        for (int j = 0; j < 4; ++j) {
            int value = info[j];
            for (int k = 0; k < 4; ++k) {
                printf("%c", (char)(value & 0xFF));
                value >>= 8;
            }
        }
    }

    // Determine processor architecture
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    const char* architecture = "Unknown";
    if (sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
        architecture = "I386";
    else if (sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
        architecture = "AMD64";

    printf("\nProcessor Architecture: %s", architecture);

    // Get vendor string
    cpuid(info, 0);
    printf("\nVendor ID: %.4s%.4s%.4s\n", (char*)&info[1], (char*)&info[3], (char*)&info[2]);

    // Get processor information
    cpuid(info, 1);

    printf("Processor Family: %d\nStepping: %d\nModel: %d\n",
           (info[0] >> 8) & 0xf,   // Processor Family
           (info[0]) & 0xf,        // Stepping
           (info[0] >> 4) & 0xf);  // Model

    // Additional feature information
    cpuid(info, 1);  // Fetch feature information again
    printf("\n==================\nFeatures:\n");

    struct Feature {
        int reg,
            bit;
        const char* name;
    };

    struct Feature features[] = {
        {3, 0, "FPU"},
        {3, 23, "MMX"},
        {3, 25, "SSE"},
        {3, 26, "SSE2"},
        {2, 0, "SSE3"},
        {2, 9, "SSSE3"},
        {2, 19, "SSE4.1"},
        {2, 20, "SSE4.2"},
        {2, 28, "AVX"},
        {2, 30, "RDTSCP"},
        {2, 9, "Virtualization"}};

    for (size_t i = 0; i < sizeof(features) / sizeof(features[0]); ++i) {
        printf(" %d) %s%s\n",
               i + 1,                                                                             // Number
               features[i].name,                                                                  // Feature name
               (info[features[i].reg] & (1 << features[i].bit)) ? " [OK]" : " [NOT AVAILABLE]");  // Status
    }

    getch();
}
