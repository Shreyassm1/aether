#include <iostream>

using namespace std;

const char* msg = "TARGET";
int d_data = 42;
int d_bss;

int main() {
    cout << "Hello, Aether!" << "\n";
    return 0;
}

// A - alloc, AX - alloc & execute, WA - write & alloc

// shreyas@DESKTOP-E7S0JNG:~/aether/learnings$ readelf -h elf_inspector | grep Entry
//   Entry point address:               0x1060
// shreyas@DESKTOP-E7S0JNG:~/aether/learnings$ readelf -S elf_inspector
// There are 31 section headers, starting at offset 0x3890:

// Section Headers:
//   [Nr] Name              Type             Address           Offset
//        Size              EntSize          Flags  Link  Info  Align
//   [ 0]                   NULL             0000000000000000  00000000
//        0000000000000000  0000000000000000           0     0     0
//   [ 1] .interp           PROGBITS         0000000000000318  00000318
//        000000000000001c  0000000000000000   A       0     0     1
//   [ 2] .note.gnu.pr[...] NOTE             0000000000000338  00000338
//        0000000000000030  0000000000000000   A       0     0     8
//   [ 3] .note.gnu.bu[...] NOTE             0000000000000368  00000368
//        0000000000000024  0000000000000000   A       0     0     4
//   [ 4] .note.ABI-tag     NOTE             000000000000038c  0000038c
//        0000000000000020  0000000000000000   A       0     0     4
//   [ 5] .gnu.hash         GNU_HASH         00000000000003b0  000003b0
//        0000000000000028  0000000000000000   A       6     0     8
//   [ 6] .dynsym           DYNSYM           00000000000003d8  000003d8
//        00000000000000d8  0000000000000018   A       7     1     8
//   [ 7] .dynstr           STRTAB           00000000000004b0  000004b0
//        0000000000000111  0000000000000000   A       0     0     1
//   [ 8] .gnu.version      VERSYM           00000000000005c2  000005c2
//        0000000000000012  0000000000000002   A       6     0     2
//   [ 9] .gnu.version_r    VERNEED          00000000000005d8  000005d8
//        0000000000000060  0000000000000000   A       7     2     8
//   [10] .rela.dyn         RELA             0000000000000638  00000638
//        00000000000000f0  0000000000000018   A       6     0     8
//   [11] .rela.plt         RELA             0000000000000728  00000728
//        0000000000000018  0000000000000018  AI       6    24     8
//   [12] .init             PROGBITS         0000000000001000  00001000
//        000000000000001b  0000000000000000  AX       0     0     4
//   [13] .plt              PROGBITS         0000000000001020  00001020
//        0000000000000020  0000000000000010  AX       0     0     16
//   [14] .plt.got          PROGBITS         0000000000001040  00001040
//        0000000000000010  0000000000000010  AX       0     0     16
//   [15] .plt.sec          PROGBITS         0000000000001050  00001050
//        0000000000000010  0000000000000010  AX       0     0     16
//   [16] .text             PROGBITS         0000000000001060  00001060
//        0000000000000126  0000000000000000  AX       0     0     16
//   [17] .fini             PROGBITS         0000000000001188  00001188
//        000000000000000d  0000000000000000  AX       0     0     4
//   [18] .rodata           PROGBITS         0000000000002000  00002000
//        000000000000001f  0000000000000000   A       0     0     4
//   [19] .eh_frame_hdr     PROGBITS         0000000000002020  00002020
//        0000000000000034  0000000000000000   A       0     0     4
//   [20] .eh_frame         PROGBITS         0000000000002058  00002058
//        00000000000000ac  0000000000000000   A       0     0     8
//   [21] .init_array       INIT_ARRAY       0000000000003da8  00002da8
//        0000000000000008  0000000000000008  WA       0     0     8
//   [22] .fini_array       FINI_ARRAY       0000000000003db0  00002db0
//        0000000000000008  0000000000000008  WA       0     0     8
//   [23] .dynamic          DYNAMIC          0000000000003db8  00002db8
//        0000000000000200  0000000000000010  WA       7     0     8
//   [24] .got              PROGBITS         0000000000003fb8  00002fb8
//        0000000000000048  0000000000000008  WA       0     0     8
//   [25] .data             PROGBITS         0000000000004000  00003000
//        0000000000000020  0000000000000000  WA       0     0     8
//   [26] .bss              NOBITS           0000000000004040  00003020
//        0000000000000118  0000000000000000  WA       0     0     64
//   [27] .comment          PROGBITS         0000000000000000  00003020
//        000000000000002b  0000000000000001  MS       0     0     1
//   [28] .symtab           SYMTAB           0000000000000000  00003050
//        0000000000000420  0000000000000018          29    21     8
//   [29] .strtab           STRTAB           0000000000000000  00003470
//        0000000000000304  0000000000000000           0     0     1
//   [30] .shstrtab         STRTAB           0000000000000000  00003774
//        000000000000011a  0000000000000000           0     0     1
// Key to Flags:
//   W (write), A (alloc), X (execute), M (merge), S (strings), I (info),
//   L (link order), O (extra OS processing required), G (group), T (TLS),
//   C (compressed), x (unknown), o (OS specific), E (exclude),
//   D (mbind), l (large), p (processor specific)
// shreyas@DESKTOP-E7S0JNG:~/aether/learnings$ objdump -d elf_inspector | grep -A 20 "<main>:"    
// 0000000000001149 <main>:
//     1149:       f3 0f 1e fa             endbr64
//     114d:       55                      push   %rbp
//     114e:       48 89 e5                mov    %rsp,%rbp
//     1151:       48 8d 05 b3 0e 00 00    lea    0xeb3(%rip),%rax        # 200b <_IO_stdin_used+0xb>
//     1158:       48 89 c6                mov    %rax,%rsi
//     115b:       48 8d 05 de 2e 00 00    lea    0x2ede(%rip),%rax        # 4040 <_ZSt4cout@GLIBCXX_3.4>
//     1162:       48 89 c7                mov    %rax,%rdi
//     1165:       e8 e6 fe ff ff          call   1050 <_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@plt>
//     116a:       48 89 c2                mov    %rax,%rdx
//     116d:       48 8d 05 a6 0e 00 00    lea    0xea6(%rip),%rax        # 201a <_IO_stdin_used+0x1a>
//     1174:       48 89 c6                mov    %rax,%rsi
//     1177:       48 89 d7                mov    %rdx,%rdi
//     117a:       e8 d1 fe ff ff          call   1050 <_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@plt>
//     117f:       b8 00 00 00 00          mov    $0x0,%eax
//     1184:       5d                      pop    %rbp
//     1185:       c3                      ret

// Disassembly of section .fini:

// 0000000000001188 <_fini>: