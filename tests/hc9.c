//
// Created by czj on 2017/7/7.
//
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <getopt.h>
#include <stdlib.h>
#include "lz4hc.h"

struct option longopts[] ={
        {"decompress", no_argument, (int *)NULL, 'd'},
        {"compress", no_argument, (int *)NULL, 'c'},
        {"input", required_argument, (int *)NULL, 'i'},
        {"output", required_argument, (int *)NULL, 'o'},
        {"mode", required_argument, (int *)NULL, 'm'},
        {"help", no_argument, (int *)NULL, 'h'},
        {(const char*)NULL ,0, (int *)NULL, 0}
};

int main(int argc, char* argv[]) {
    int retval = 0;
    FILE *out, *in;
    char *inbuf, *outbuf;
    char c;
    int isCompress = 0;
    int mode = 9;
    char infile[512] = {0}, outfile[512] = {0};
    const char *opts = ":dci:o:m:h:";
    while((c = getopt_long(argc, argv, opts, longopts, NULL)) != -1) {
        switch(c) {
            case 'd':
                isCompress = 0;
                break;
            case 'c':
                isCompress = 1;
                break;
            case 'i':
                printf("input file is %s\n",optarg);
                strcpy(infile, optarg);
                break;
            case 'o':
                printf("output file is %s\n",optarg);
                strcpy(outfile, optarg);
                break;
            case 'm':
                printf("mode is %s\n",optarg);
                sscanf(optarg, "%d",&mode);
                break;
            case '?':
                printf("argument error!\n");
            case 'h':
            default:
//                printf("Usage: ccks-tool [OPTION]...\n");
//                printf("\t-d,--pack\t\tpack file path.\n");
//                printf("\t-k,--keystore\t\tid file path\n");
//                printf("\t-i,--input\t\tinput file path\n");
//                printf("\t-o,--output\t\toutput file path\n");
//                printf("\t-m,--mode\t\tsign check encrypt decrypt makekey\n");
                return -1;
        }
    }
    printf("mode=%d, input=[%s], output=[%s], isCompress=%d\n", mode, infile, outfile, isCompress);
    in = fopen(infile, "rb");
    out = fopen(outfile, "wb");
    if(in == NULL || out == NULL){
        return 1;
    }
    fseek(in, 0 ,SEEK_END);
    int inFileSize = (int)ftell(in);
    fseek(in, 0 ,SEEK_SET);
    inbuf = (char*)malloc((size_t)inFileSize);
    size_t readCount = fread(inbuf, (size_t)inFileSize, 1, in);
    if(readCount != 1) {
        retval = 2;
        goto exit;
    }
    if(isCompress) { //压缩
        int compressSize = LZ4_compressBound(inFileSize);
        outbuf = (char*)malloc((size_t)compressSize);
        int destSize = LZ4_compress_HC(inbuf, outbuf, inFileSize, compressSize, mode);
        fwrite(&inFileSize, 4, 1, out);
        fwrite(&destSize, 4, 1, out);
        fwrite(outbuf, (size_t)destSize, 1, out);
        fflush(out);
    }
    else { //解压缩
        fread(inbuf, 8, 1, in);
        int deSize = *((int*)inbuf);
        deSize++;
        int comSize = *((int*)(inbuf+4));
        fread(inbuf+8, comSize, 1, in);
        outbuf = (char*)malloc(deSize);
        int sz = LZ4_decompress_safe(inbuf+8, outbuf, comSize, deSize);
        fwrite(outbuf, sz, 1, out);
        fflush(out);
    }

    exit:
    fclose(in);
    fclose(out);
    return retval;
}