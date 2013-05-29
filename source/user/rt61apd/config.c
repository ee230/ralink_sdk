/*
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. See README and COPYING for
 * more details.

    Module Name:
    config.c

    Revision History:
    Who         When          What
    --------    ----------    ----------------------------------------------
    Jan, Lee    Dec --2003    modified

*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "rt61apd.h"
#include "md5.h"

unsigned char BtoH(
    unsigned char ch)
{
    if (ch >= '0' && ch <= '9') return (ch - '0');        // Handle numerals
    if (ch >= 'A' && ch <= 'F') return (ch - 'A' + 0xA);  // Handle capitol hex digits
    if (ch >= 'a' && ch <= 'f') return (ch - 'a' + 0xA);  // Handle small hex digits
    return(255);
}

//
//  PURPOSE:  Converts ascii string to network order hex
//
//  PARAMETERS:
//    src    - pointer to input ascii string
//    dest   - pointer to output hex
//    destlen - size of dest
//
//  COMMENTS:
//
//    2 ascii bytes make a hex byte so must put 1st ascii byte of pair
//    into upper nibble and 2nd ascii byte of pair into lower nibble.
//
void AtoH(
    char            *src,
    unsigned char	*dest,
    int		        destlen)
{
    char *srcptr;
    unsigned char *destTemp;

    srcptr = src;   
    destTemp = (unsigned char *) dest; 

    while(destlen--)
    {
        *destTemp = BtoH(*srcptr++) << 4;    // Put 1st ascii byte in upper nibble.
        *destTemp += BtoH(*srcptr++);      // Add 2nd ascii byte to above.
        destTemp++;
    }
}

/**
 * rstrtok - Split a string into tokens
 * @s: The string to be searched
 * @ct: The characters to search for
 * * WARNING: strtok is deprecated, use strsep instead. However strsep is not compatible with old architecture.
 */
char * __rstrtok;
char * rstrtok(char * s,const char * ct)
{
	char *sbegin, *send;

	sbegin  = s ? s : __rstrtok;
	if (!sbegin)
	{
		return NULL;
	}

	sbegin += strspn(sbegin,ct);
	if (*sbegin == '\0')
	{
		__rstrtok = NULL;
		return( NULL );
	}

	send = strpbrk( sbegin, ct);
	if (send && *send != '\0')
		*send++ = '\0';

	__rstrtok = send;

	return (sbegin);
}

static int
Config_read_radius_addr(struct hostapd_radius_server **server,
                int *num_server, const char *val, int def_port,
                struct hostapd_radius_server **curr_serv)
{
    struct hostapd_radius_server *nserv;
    int ret;

    nserv = realloc(*server, (*num_server + 1) * sizeof(*nserv));
    if (nserv == NULL)
        return -1;

    *server = nserv;
    nserv = &nserv[*num_server];
    (*num_server)++;
    (*curr_serv) = nserv;

    memset(nserv, 0, sizeof(*nserv));
    nserv->port = def_port;
    ret = !inet_aton(val, &nserv->addr);

    return ret;
}

struct rtapd_config * Config_read(const char *fname, int pid)
{
    struct rtapd_config *conf;
    FILE *f;
    char buf[1600], *pos, *tokpos;
    int line = 0/*,  savecur = 0,tmp = 0*/;
    int errors = 0, i = 0;
    int flag = 0;
    long    filesize/*,cur = 0*/;
    char    *ini_buffer/*,save,prev,*buff*/;             /* storage area for .INI file */
//    int now = 100000;

    f = fopen(fname, "r");
    //DBGPRINT(RT_DEBUG_TRACE, "r + b     %x \n",'\n');
    if (f == NULL)
    {
        DBGPRINT(RT_DEBUG_ERROR,"Could not open configuration file '%s' for reading.\n", fname);
        return NULL;
    }
    if ((fseek(f, 0, SEEK_END))!=0)
        return (0);

    filesize=ftell(f);
    if ((ini_buffer=(char *)malloc(filesize + 1 + 10))==NULL)
        return (0);   //out of memory

    fseek(f,0,SEEK_SET);
    fread(ini_buffer, filesize, 1, f);
    fseek(f,0,SEEK_SET);
    ini_buffer[filesize]='\0';
    conf = malloc(sizeof(*conf));
    if (conf == NULL)
    {
        DBGPRINT(RT_DEBUG_TRACE, "Failed to allocate memory for configuration data.\n");
        fclose(f);
        return NULL;
    }
    memset(conf, 0, sizeof(*conf));

    conf->SsidNum = 1;
    conf->session_timeout_set = 0xffff;
    
    // initial default shared-key material and index
    for (i = 0; i < MAX_MBSSID_NUM; i++)
    {
		conf->DefaultKeyID[i] = 0;										// broadcast key index
		conf->individual_wep_key_idx[i] = 3;							// unicast key index 
    	conf->individual_wep_key_len[i] = WEP8021X_KEY_LEN;				// key length
    	hostapd_get_rand(conf->IEEE8021X_ikey[i], WEP8021X_KEY_LEN);    // generate shared key randomly
  	}
  
	// initial default EAP IF name and Pre-Auth IF name	as "br0"
	strcpy(conf->EAPifname, "br0");
	strcpy(conf->PreAuthifname, "br0");
  
    while ((fgets(buf, sizeof(buf), f)))
    {
        line++;
        if (buf[0] == '#')
            continue;
        pos = buf;
        while (*pos != '\0')
        {
            if (*pos == '\n')
            {
                *pos = '\0';
                break;
            }
            pos++;
        }
        if (buf[0] == '\0')
            continue;

        pos = strchr(buf, '=');
        if (pos == NULL)
        {
            pos = strstr(buf, "Default");
            if (pos == NULL) 
                errors++;
            continue;
        }
        *pos = '\0';
        pos++;

        if (strcmp(buf, "debug") == 0)
        {
            conf->debug = atoi(pos);
        }
        else if (strcmp(buf, "own_ip_addr") == 0)
        {
            if (!inet_aton(pos, &conf->own_ip_addr))
            {
                DBGPRINT(RT_DEBUG_ERROR,"Line %d: invalid IP address '%s'\n", line, pos);
                errors++;
            }
            flag |= 0x01;
        }
        else if (strcmp(buf, "RADIUS_Server") == 0)
        {
#if MULTIPLE_RADIUS  
			DBGPRINT(RT_DEBUG_TRACE, "RADIUS_Server: '%s'\n", pos);
			for (i = 0, tokpos = rstrtok(pos,";"); tokpos && (i < MAX_MBSSID_NUM); tokpos = rstrtok(NULL,";"), i++)
			{
				if (Config_read_radius_addr(
                    &conf->mbss_auth_servers[i],
                    &conf->mbss_num_auth_servers[i], tokpos, 1812,
                    &conf->mbss_auth_server[i]))
            	{
                	DBGPRINT(RT_DEBUG_ERROR, "Line %d: invalid IP address '%s'\n", line, tokpos);
	                errors++;
    	        }      
				DBGPRINT(RT_DEBUG_TRACE, "		IP address: '%s' for raL%d\n", tokpos, i);
			}			
#else			
            if (Config_read_radius_addr(
                    &conf->auth_servers,
                    &conf->num_auth_servers, pos, 1812,
                    &conf->auth_server))
            {
                DBGPRINT(RT_DEBUG_ERROR,"Line %d: invalid IP address '%s'\n", line, pos);
                errors++;
            }
            DBGPRINT(RT_DEBUG_TRACE, "Radius IP address: '%s'\n", pos);
#endif			
            flag |= 0x02;
        }
#if MULTIPLE_RADIUS        
		else if (strcmp(buf, "RADIUS_Port") == 0)   
#else        
        else if (conf->auth_server && strcmp(buf, "RADIUS_Port") == 0)
#endif        
        {
#if MULTIPLE_RADIUS   
			DBGPRINT(RT_DEBUG_TRACE, "RADIUS_Port: '%s'\n", pos);
			for (i = 0, tokpos = rstrtok(pos,";"); tokpos && (i < MAX_MBSSID_NUM); tokpos = rstrtok(NULL,";"), i++)
			{	
				if (conf->mbss_auth_server[i])
				{
					conf->mbss_auth_server[i]->port = atoi(tokpos);	            	
		            DBGPRINT(RT_DEBUG_TRACE,"RADIUS_Port: '%s' for raL%d\n", tokpos, i);
				}
			}
			flag |= 0x04;
#else
            conf->auth_server->port = atoi(pos);
            flag |= 0x04;
            DBGPRINT(RT_DEBUG_TRACE,"RADIUS_Port: '%s'\n", pos);
#endif			
        }
#if MULTIPLE_RADIUS 
       else if (strcmp(buf, "RADIUS_Key") == 0)
#else        
       else if (conf->auth_server && strcmp(buf, "RADIUS_Key") == 0)
#endif       
        {
#if MULTIPLE_RADIUS
			DBGPRINT(RT_DEBUG_TRACE, "RADIUS_Key: '%s'\n", pos);
			for (i = 0, tokpos = rstrtok(pos,";"); tokpos && (i < MAX_MBSSID_NUM); tokpos = rstrtok(NULL,";"), i++)
			{	
				if (conf->mbss_auth_server[i])
				{
					int len = strlen(tokpos);
		            if (tokpos[len-1] == 0xd)
        		        len--;
		            if ( len == 0 || len == 1)
        		    {
		                /* RFC 2865, Ch. 3 */
        		        DBGPRINT(RT_DEBUG_ERROR,"Line %d: empty shared secret is not allowed.\n", line);
		                errors++;
        		    }
					conf->mbss_auth_server[i]->shared_secret = strdup(tokpos);            
            		conf->mbss_auth_server[i]->shared_secret_len = len;
            		DBGPRINT(RT_DEBUG_TRACE,"RADIUS_Key: '%s', Key_len: %d for raL%d\n", 
						conf->mbss_auth_server[i]->shared_secret, conf->mbss_auth_server[i]->shared_secret_len, i);								
				}
			}
			flag |= 0x08;
#else
            int len = strlen(pos);
            if (pos[len-1] == 0xd)
                len--;
            if ( len == 0 || len == 1)
            {
                /* RFC 2865, Ch. 3 */
                DBGPRINT(RT_DEBUG_ERROR,"Line %d: empty shared secret is not allowed.\n", line);
                errors++;
            }
            conf->auth_server->shared_secret = strdup(pos);            
            conf->auth_server->shared_secret_len = len;
            DBGPRINT(RT_DEBUG_TRACE,"RADIUS_Key: '%s', Key_len: %d\n", conf->auth_server->shared_secret, conf->auth_server->shared_secret_len);
            flag |= 0x08;
#endif			
        }
        else if (strcmp(buf, "radius_retry_primary_interval") == 0)
        {
            conf->radius_retry_primary_interval = atoi(pos);
        }
        else if (strcmp(buf, "BssidNum") == 0)
        {
            conf->SsidNum = atoi(pos);
            if(conf->SsidNum > MAX_MBSSID_NUM)
                conf->SsidNum = 1;
            DBGPRINT(RT_DEBUG_TRACE,"conf->SsidNum=%d\n", conf->SsidNum);
        }
        else if (strcmp(buf, "DefaultKeyID") == 0)
        {
            DBGPRINT(RT_DEBUG_TRACE,"DefaultKeyID: '%s'\n", pos);
            for (i = 0, tokpos = rstrtok(pos,";"); tokpos && (i < MAX_MBSSID_NUM); tokpos = rstrtok(NULL,";"), i++)
            {
            	int KeyIdx;
            
            	KeyIdx = atoi(tokpos);
            	if((KeyIdx >= 1 ) && (KeyIdx <= 4))
            	{
            		conf->DefaultKeyID[i] = KeyIdx - 1;
            	
            		if (conf->DefaultKeyID[i] == 3)
            			conf->individual_wep_key_idx[i] = 0;
            		else
            			conf->individual_wep_key_idx[i] = 3;	
            	
            		DBGPRINT(RT_DEBUG_TRACE,"conf->DefaultKeyID[%d] = %d\n", i, conf->DefaultKeyID[i]);
            	}
            	else
            	{
                	DBGPRINT(RT_DEBUG_ERROR,"Line %d: DefaultKeyID must be 1~4 for raL%d.\n", line, i);
                  errors++;		  
            }
        }
        }
        else if (strcmp(buf, "Key1Str") == 0)
        {
            DBGPRINT(RT_DEBUG_TRACE,"Key1Str: '%s'\n", pos);
            
            for (i = 0, tokpos = rstrtok(pos,";"); tokpos && (i < MAX_MBSSID_NUM); tokpos = rstrtok(NULL,";"), i++)
            {
            	// Use wep key1 material as shared key 
            	if (conf->DefaultKeyID[i] == 0)
            	{
            		int	KeyLen = 0;
            
            		KeyLen = strlen(tokpos);
	        		if ((KeyLen == 5) || (KeyLen == 13))         //Ascii
	        		{
                		conf->individual_wep_key_len[i] = KeyLen;
                		memset(conf->IEEE8021X_ikey[i], 0, WEP8021X_KEY_LEN);
                		memcpy(conf->IEEE8021X_ikey[i], tokpos, strlen(tokpos));
                		
                		DBGPRINT(RT_DEBUG_TRACE,"iface(raL%d): use Key1Str '%s' as Default Key, Key_len: %d and Type is Ascii.\n", i, tokpos, KeyLen);
	        		}
	        		else if ((KeyLen == 10) || (KeyLen == 26))   //Hex
	        		{
			    		conf->individual_wep_key_len[i] = KeyLen/2;
			    		memset(conf->IEEE8021X_ikey[i], 0, WEP8021X_KEY_LEN);
                		AtoH(tokpos, (unsigned char *)conf->IEEE8021X_ikey[i], strlen(tokpos)/2);	  
                		
                		DBGPRINT(RT_DEBUG_TRACE,"iface(raL%d): use Key1Str '%s' as Default Key, Key_len: %d and Type is Hex.\n", i, tokpos, KeyLen);
	        		}
        		}
        	}	
        }
        else if (strcmp(buf, "Key2Str") == 0)
        {
            DBGPRINT(RT_DEBUG_TRACE,"Key2Str: '%s'\n", pos);
            
            for (i = 0, tokpos = rstrtok(pos,";"); tokpos && (i < MAX_MBSSID_NUM); tokpos = rstrtok(NULL,";"), i++)
            {
            	// Use wep key2 material as shared key 
            	if (conf->DefaultKeyID[i] == 1)
            	{
            		int	KeyLen = 0;
            
            		KeyLen = strlen(tokpos);
	        		if ((KeyLen == 5) || (KeyLen == 13))         //Ascii
	        		{
                		conf->individual_wep_key_len[i] = KeyLen;
                		memset(conf->IEEE8021X_ikey[i], 0, WEP8021X_KEY_LEN);
                		memcpy(conf->IEEE8021X_ikey[i], tokpos, strlen(tokpos));
                		
                		DBGPRINT(RT_DEBUG_TRACE,"iface(raL%d): use Key2Str '%s' as Default Key, Key_len: %d and Type is Ascii.\n", i, tokpos, KeyLen);
	        		}
	        		else if ((KeyLen == 10) || (KeyLen == 26))   //Hex
	        		{
			    		conf->individual_wep_key_len[i] = KeyLen/2;
			    		memset(conf->IEEE8021X_ikey[i], 0, WEP8021X_KEY_LEN);
                		AtoH(tokpos, (unsigned char *)conf->IEEE8021X_ikey[i], strlen(tokpos)/2);	  
                		
                		DBGPRINT(RT_DEBUG_TRACE,"iface(raL%d): use Key2Str '%s' as Default Key, Key_len: %d and Type is Hex.\n", i, tokpos, KeyLen);
	        		}
        		}
        	}	
        }
        else if (strcmp(buf, "Key3Str") == 0)
        {
            DBGPRINT(RT_DEBUG_TRACE,"Key3Str: '%s'\n", pos);
            
            for (i = 0, tokpos = rstrtok(pos,";"); tokpos && (i < MAX_MBSSID_NUM); tokpos = rstrtok(NULL,";"), i++)
        {
            	// Use wep key3 material as shared key 
            	if (conf->DefaultKeyID[i] == 2)
	        {
            		int	KeyLen = 0;
            
            		KeyLen = strlen(tokpos);
	        		if ((KeyLen == 5) || (KeyLen == 13))         //Ascii
	        		{
                		conf->individual_wep_key_len[i] = KeyLen;
                		memset(conf->IEEE8021X_ikey[i], 0, WEP8021X_KEY_LEN);
                		memcpy(conf->IEEE8021X_ikey[i], tokpos, strlen(tokpos));
                		
                		DBGPRINT(RT_DEBUG_TRACE,"iface(raL%d): use Key3Str '%s' as Default Key, Key_len: %d and Type is Ascii.\n", i, tokpos, KeyLen);
	        }
	        		else if ((KeyLen == 10) || (KeyLen == 26))   //Hex
	        {
			    		conf->individual_wep_key_len[i] = KeyLen/2;
			    		memset(conf->IEEE8021X_ikey[i], 0, WEP8021X_KEY_LEN);
                		AtoH(tokpos, (unsigned char *)conf->IEEE8021X_ikey[i], strlen(tokpos)/2);	  
                		
                		DBGPRINT(RT_DEBUG_TRACE,"iface(raL%d): use Key3Str '%s' as Default Key, Key_len: %d and Type is Hex.\n", i, tokpos, KeyLen);
	        }
        		}
        	}	
        }
        else if (strcmp(buf, "Key4Str") == 0)
        {
            DBGPRINT(RT_DEBUG_TRACE,"Key4Str: '%s'\n", pos);
            
            for (i = 0, tokpos = rstrtok(pos,";"); tokpos && (i < MAX_MBSSID_NUM); tokpos = rstrtok(NULL,";"), i++)
            {
            	// Use wep key4 material as shared key 
            	if (conf->DefaultKeyID[i] == 3)
            	{
            		int	KeyLen = 0;
            
            		KeyLen = strlen(tokpos);
	        		if ((KeyLen == 5) || (KeyLen == 13))         //Ascii
	        		{
                		conf->individual_wep_key_len[i] = KeyLen;
                		memset(conf->IEEE8021X_ikey[i], 0, WEP8021X_KEY_LEN);
                		memcpy(conf->IEEE8021X_ikey[i], tokpos, strlen(tokpos));
            
                		DBGPRINT(RT_DEBUG_TRACE,"iface(raL%d): use Key4Str '%s' as Default Key, Key_len: %d and Type is Ascii.\n", i, tokpos, KeyLen);
        }
	        		else if ((KeyLen == 10) || (KeyLen == 26))   //Hex
	        		{
			    		conf->individual_wep_key_len[i] = KeyLen/2;
			    		memset(conf->IEEE8021X_ikey[i], 0, WEP8021X_KEY_LEN);
                		AtoH(tokpos, (unsigned char *)conf->IEEE8021X_ikey[i], strlen(tokpos)/2);	  

                		DBGPRINT(RT_DEBUG_TRACE,"iface(raL%d): use Key4Str '%s' as Default Key, Key_len: %d and Type is Hex.\n", i, tokpos, KeyLen);
	        		}
        		}
        	}	
        }	
/*
        else if ((strcmp(buf, "pid") == 0) && (pid != 0))
        {
            //fseek(f,0,SEEK_CUR);
            //fprintf(f, "pid=%4x", pid);
            flag |= 0x10;
            cur = 0;
            tmp = pid;
            while(cur <= (int)filesize)
            {  
                if ((ini_buffer[cur]=='p') && (ini_buffer[cur+1]=='i') && (ini_buffer[cur+2]=='d') )
                {
                    cur += 4;
                    for( i=4; i>=0; i--)
                    {
                        now = now/10;
                        if(ini_buffer[cur]!='\n')
                        {
                            ini_buffer[cur] = tmp/(now)+0x30;
                        }
                        else
                        {
                            prev = ini_buffer[cur];
                            ini_buffer[cur] = tmp/(now)+0x30;
                            savecur = cur+1;
                            do
                            {
                                save = ini_buffer[savecur];
                                ini_buffer[savecur] = prev;
                                prev = save;
                                savecur ++;
                            }while(savecur <= (filesize+5));
                        }  
                        cur++;
                        tmp -= ((tmp/(now))*(now));                        
                    }   
                    break;
                }
                cur++;
            }
        } 
*/        
        else if (strcmp(buf, "session_timeout_interval") == 0)
        {
            flag |= 0x20;
            conf->session_timeout_interval = atoi(pos);
            if (conf->session_timeout_interval == 0)
                conf->session_timeout_set= 0;
            else
                conf->session_timeout_set= 1;
		
            DBGPRINT(RT_DEBUG_TRACE,"session_timeout policy = %s \n", conf->session_timeout_set?"set":"not use");
            DBGPRINT(RT_DEBUG_TRACE,"Read Session Timeout Interval  %d seconds. \n", conf->session_timeout_interval);
            conf->session_timeout_interval = (atoi(pos)<60) ? REAUTH_TIMER_DEFAULT_reAuthPeriod : atoi(pos);
            DBGPRINT(RT_DEBUG_TRACE,"Set Session Timeout Interval  %d seconds. \n", conf->session_timeout_interval);
        }
        else if (strcmp(buf, "EAPifname") == 0)
        {
        	int len = strlen(pos);
            if (pos[len-1] == 0xd)
                len--;
            if (len > 0)            
            {
				memset(conf->EAPifname, 0, IFNAMSIZ);	
				memcpy(conf->EAPifname, pos, len);	            
            }
			DBGPRINT(RT_DEBUG_TRACE,"EAPifname: %s \n", conf->EAPifname);
        }
		else if (strcmp(buf, "PreAuthifname") == 0)
        {
        	int len = strlen(pos);
            if (pos[len-1] == 0xd)
                len--;
            if (len > 0)            				
			{						
				memset(conf->PreAuthifname, 0, IFNAMSIZ);
				memcpy(conf->PreAuthifname, pos, len);    	        
            }
			DBGPRINT(RT_DEBUG_TRACE,"PreAuthifname: %s \n", conf->PreAuthifname);
        }
    }
        
    fseek(f,0,SEEK_SET);
    fprintf(f, "%s", ini_buffer);    
    fclose(f);
#if 0
    if ((!(flag&0x10)) && (pid != 0))
    {
        tmp = pid;
        f = fopen(fname, "r+");
        if ((buff=(char *)malloc(10))==NULL)
            return (0);   //out of memory
        buff[9]='\0';
        fseek(f,0,SEEK_END);
        strcpy(buff,"pid=");
        cur = 4;
        for( i=4; i>=0; i--)
        {
            now = now/10;
            buff[cur] = tmp/(now)+0x30;
            cur++;
            tmp -= ((tmp/(now))*(now));
        }        
        fprintf(f, buff);    
        fclose(f);
    }
#endif

#if MULTIPLE_RADIUS
	for (i = 0; i < MAX_MBSSID_NUM; i++)
	{
		struct hostapd_radius_server *servs, *cserv, *nserv;
		int c;

		conf->mbss_auth_server[i] = conf->mbss_auth_servers[i];

		if (!conf->mbss_auth_server[i])
			continue;
						
		cserv	= conf->mbss_auth_server[i];
		servs 	= conf->mbss_auth_servers[i];								
			
		DBGPRINT(RT_DEBUG_TRACE, "raL%d, Current IP: %s \n", i, inet_ntoa(cserv->addr));			
		for (c = 0; c < conf->mbss_num_auth_servers[i]; c++)
		{				
			nserv = &servs[c];             
			DBGPRINT(RT_DEBUG_TRACE, "	   Server IP List: %s \n", inet_ntoa(nserv->addr));
		}				
	}
#else
    conf->auth_server = conf->auth_servers;
#endif
	
    if (errors )
    {
        DBGPRINT(RT_DEBUG_ERROR,"%d errors found in configuration file '%s'\n", errors, fname);
        Config_free(conf);
        conf = NULL;
    }
    if ((flag&0x0f)!=0x0f )
    {
        DBGPRINT(RT_DEBUG_ERROR,"Not enough parameters found in configuration file '%s'\n", fname);
        Config_free(conf);
        conf = NULL;
    }
    return conf;
}

static void Config_free_radius(struct hostapd_radius_server *servers, int num_servers)
{
    int i;

    for (i = 0; i < num_servers; i++)
    {
        free(servers[i].shared_secret);
    }
    free(servers);
}

void Config_free(struct rtapd_config *conf)
{
#if MULTIPLE_RADIUS
	int	i;
#endif
	
    if (conf == NULL)
        return;

#if MULTIPLE_RADIUS
	for (i = 0; i < MAX_MBSSID_NUM; i++)
	{
		if (conf->mbss_auth_servers[i])
			Config_free_radius(conf->mbss_auth_servers[i], conf->mbss_num_auth_servers[i]);
	}
#else
    Config_free_radius(conf->auth_servers, conf->num_auth_servers);
#endif
    free(conf);
}

