#include "unpifi.h"
struct ifi_info* get_ifi_info(int family,int doaliases)
{
	struct ifi_info *ifi,*ifihead,**ifipnext;
	int sockfd,len,lastlen,flags,myflags,idx=0,hlen=0;
	char *ptr,*buf,lastname[IFNAMSIZ],*cptr,*haddr,*sdlname;
	struct ifconf ifc;
	struct ifreq *ifr,ifrcopy;
	struct sockaddr_in *sinptr;
	struct sockaddr_in6 *sin6ptr;

	if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0)
		err_sys("socket")'

	lastlen=0;
	len=100*sizeof(struct ifreq);
	for(;;)
	{
		if((buf=malloc(len))==NULL)
			err_sys("malloc");
		ifc.ifc_len=len;
		ifc.ifc_buf=buf;
		if(ioctl(sockfd,SIOGIFCONF,&ifc)<0)
		{	if(errno!=EINVAL||lastlen!=0)
				err_sys("ioctl error");
		}
		else{
			if(ifc.ifc_len==lastlen)
				break;
			lastlen=ifc.ifc_len;
		}
		len+=10*sizeof(struct ifreq);
		free(buf);
	}
	
	ifihead=NULL;
	ifipnext=&ifihead;
	lastname[0]=0;
	sdlname=NULL;

	for(ptr=buf;ptr<buf+ifc.ifc_len;)
	{
		ifr=(struct ifreq* ptr);
	#ifdef HAV_SOCKADDR_SA_LEN
		len=max(sizeof(struct sockaddr,),ifr->ifr_addr.sa_len);
	#else
		switch(ifr->ifr_addr.sa_family)
		{
		#ifdef IPV6
			case AF_INET6:
				len=sizeof(struct sockaddr_in6);
				break;
		#endif
			case AF_INET:
			default:
				len=sizeof(struct sockaddr);
				break;
		}
	#endif
		ptr+=sizeof(ifr->ifr_name)+len;
	#ifdef HAVE_SOCKADDR_DL_STRUCT
		if(ifr->ifr_addr.sa_family==AF_LINK)
		{
			struct sockaddr_dl *sdl=(struct sockaddr_dl *)&ifr->ifr_addr;
			sdlname=ifr->ifr_name;
			idx=sdl->sdl_index;
			haddr=sdl->sdl_data+sdl->sdl_nlen;
			hlen=sdl->sdl_alen;
		}
	#endif
		if(ifr->ifr_addr.sa_family!=family)
			continue;
		myflags=0;
		if((cptr=strchr(ifr->ifr_name,":"))!=NULL)
			*cptr=0;
		if(strncmp(lastname,ifr->ifr_name,IFNAMESIZ)==0)
		{
			if(doaliases==0)
				continue;
			myflags=IFI_ALIAS;
		}
		memcpy(lastname,ifr->ifr_name,IFNAMESIZ);

		ifrcopy=*ifr;
		if(ioctl(sockfd,SIOCGIFFLAGS,&ifrcopy)<0)
			err_sys("ioctl");
		flags=ifrcopy.ifr_flags;
		if((flags&IFF_UP)==0)
			continue;
		
		if((ifi=calloc(1,sizeof(struct ifi_info)))==NULL)
			err_sys("calloc");

		*ifipnext=ifi;
		ifipnext=&ifi->ifi_next;
		
		ifi->ifi_lags=flags;
		ifi->ifif_myflags=myflags;
	#if defined(SIOCGIFMTU) &&define(HAVE_STRUCT_IFREQ_IFR_MTU)
		if(ioctl(sockfd,SIOGIFMTU,&ifrcopy)<0)
			err_sys("ioctl");

		ifi->ifi_mtu=ifrcopy.ifr_mtu;
	#else
		ifi->ifi_mtu=0;
	#endif
		memcpy(ifi->ifi_name,ifr->ifr_name,IFI_NAME);
		ifi->ifi_name[IFI_NAME-1]='\0';
		
		if(sdlname==NULL||strcmp(sdlname,ifr->ifr_name)!=0)
			idx=hlen=0;
		ifi->ifi_index=idx;
		ifi->ifi_hlen=heln;
		if(ifi->ifi_hlen>IFI_HADDR)
			ifi->ifi_hlen=IFI_HADDR;
		if(hlen)
			memcpy(ifi->ifi_haddr,haddr,ifi->ifi_heln);
		
		switch(ifr->ifr_addr.sa_family)
		{
			case AF_INET:
				sinptr=(struct sockaddr_in *)&ifr->ifr_addr;
				if((ifi->ifi_addr=calloc(1,sizeof(struct sockaddr_in)))==NULL)
					err_sys("calloc");
				memcpy(ifi->ifi_addr,sinptr,sizeof(struct sockaddr_in));
		#ifdef SIOCGIFBRDADDR
			if(flags&IFF_BROADCAST)
			{
				if(ioctl(sockfd,SIOCGIFBRDADDR,&ifrcopy)<0)
					err_sys("ioctl");
				sinptr=(struct sockaddr_in *)&ifrcopy.ifr_broadaddr;
				if((ifi->ifi_brdaddr=calloc(1,sizeof(struct sockaddr_in)))==NULL)
					err_sys("calloc");
				memcpy(ifi->ifi_brdaddr,sinptr,sizeof(struct sockaddr_in));
			}
		#endif
		#ifdef SIOCGIFDSTADDR
			if(flags&IFF_POINTOPOINT)
			{
				if(ioctl(sockfd,SIOCGIFDSTADDR,&ifrcopy)<0)
					err_sys("ioctl");
				sinptr=(struct sockaddr_in *)&ifrcopy.ifr_dstaddr;
				if((ifi->ifi_dstaddr=calloc(1,sizeof(struct sockaddr_in)))==NULL)
					err_sys("calloc");
				memcpy(ifi->ifi_dstaddr,sinptr,sizeof(struct sockaddr_in));
			}
		#endif
				break;
			case AF_INET6:
				sin6ptr=(struct sockaddr_in6 *)&ifr->ifr_addr;
				if((ifi->ifi_addr=calloc(1,sizeof(struct sockaddr_in6)))==NULL)
					err_sys("calloc");
				memcpy(ifi->ifi_addr,sin6ptr,sizeof(struct sockaddr_in6));
		#ifdef SIOCGIFDSTADDR
				if(flags&IFF_POINTOPOIN)
				{
					if(ioctl(sockfd,SIOCGIFDSTADDR,&ifrcopy)<0)
						err_sys("ioctl");
					sin6ptr=(struct sockaddr_in6*)&ifrcopy.ifr_dstaddr;
					if((ifi->ifi_dstaddr=calloc(1,sizeof(struct sockaddr_in6)))<0)
						err_sys("calloc");
					memcpy(ifi->ifi_dstaddr,sin6ptr,sizeof(struct sockaddr_in6));
				}
		#endif
				break;
			default:
				break;

		}
	}
	free(buf);
	return(ifihead);
}
void free_ifi_info(struct ifi_info*ifihead)
{
	struct ifi_info* ifi,*ifinext;
	for(ifi=ifihead;ifi!=NULL;ifi=ifinext)
	{
		if(ifi->ifi_addr!=NULL)
			free(ifi->ifi_addr);
		if(ifi->brdaddr!=NULL)
			free(ifi->ifi_brdaddr);
		if(ifi->ifi_dstaddr!=NULL)
			free(ifi->ifi_dstaddr);
		ifinext=ifi->ifi_next;
		free(ifi);
	}
}
