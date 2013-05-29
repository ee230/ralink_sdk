#ifndef HTTPENCODE_H
#define HTTPENCODE_H

#define REQ_URL_P1_FMT "GET %s?info_hash=%s&peer_id=%s&port=%d&key=%s"
#define REQ_URL_P1A_FMT "GET %s&info_hash=%s&peer_id=%s&port=%d&key=%s"
#define REQ_URL_P2_FMT "%s%s%s&uploaded=%llu&downloaded=%llu&left=%llu&compact=1&numwant=%d HTTP/1.0"


char* Http_url_encode(char *s,const char *b,size_t n);
int Http_url_analyse(const char *url,char *host,int *port,char *path);
size_t Http_split(char *b,size_t n,char **pd,size_t *dlen);
int Http_reponse_code(const char *b,size_t n);
int Http_get_header(const char *b,int n,const char *header,char *v);

#endif
