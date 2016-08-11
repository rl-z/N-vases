#include <Rcpp.h>
using namespace Rcpp;

// [[Rcpp::export]]

IntegerVector Csimulate_2(int chains, int backlog, int hpm, int max_time,int iterations, int seed) {

/* this function returns a vector of catchup times, */
/* within max_time, repeated iterations times       */
/* given fixed chains, backlog and hashpower miner  */
/* catchup time = 0 when not catched up             */

  int situation_backlogs[chains];
  int pick_chain = 0;
  int ix;
  double miner_hp;
  double attacker_hp;
  double miner_rnd;
  double attacker_rnd;
  bool catchup_succeeded = false;
  IntegerVector out(iterations,0);
  IntegerVector::iterator it_out;

  srand(seed);
  miner_hp = (double) hpm/100;
  attacker_hp = 1.0 - miner_hp;


  for (it_out=out.begin();it_out!=out.end();++it_out){
    for (int ixx = 0; ixx<chains; ixx++){situation_backlogs[ixx]=backlog;}
    catchup_succeeded = false;

    for(int ix_time = 1; ix_time <= max_time; ++ix_time) {
      ix = ix_time;
      miner_rnd = ((double) rand() / (RAND_MAX));
      if (miner_rnd <= miner_hp) {
        pick_chain = rand() % chains;
        situation_backlogs[pick_chain]++;}
      attacker_rnd = ((double) rand() / (RAND_MAX));
      if (attacker_rnd <= attacker_hp) {
        pick_chain = rand() % chains;
        situation_backlogs[pick_chain]--;}
      /* check if attacker has catched up */
      catchup_succeeded = false;
      for (int ixx = 0; ixx<chains; ixx++){
        if (situation_backlogs[ixx]==0) {
          catchup_succeeded = true;
          break;}
      }
      if (catchup_succeeded) {break;}
    } /* end loop time */
    if (catchup_succeeded) {*it_out = ix;} else {*it_out=0;}
  } /* end loop iteration */

  return(out);
}


// You can include R code blocks in C++ files processed with sourceCpp
// (useful for testing and development). The R code will be automatically
// run after the compilation.
//

/*** R
gc()
library(parallel) # we apply a multicore solution
mode2<-FALSE
mode2a<-!mode2
if (mode2){
  par(mfrow=c(2,2))
  CH<-c(32)
  BL<-c(5)
  HPM<-c(70)
  lout<-40
  IT<-seq(from=100,by=100,length.out=lout)
  MT<-seq(from=1000,by=1000,length.out=lout)

  chains<-CH[1]
  backlog<-BL[1]
  hpm<-HPM[1]

  L<-list()

  EG<-expand.grid(CH,BL,HPM,MT,IT)
  N<-nrow(EG)
  brk<-50
  time.start=proc.time()
  #Csimulate_2(int chains, int backlog, int hpm, int max_time,int iterations
  result_mc<-mcmapply(Csimulate_2,EG[,1],EG[,2],EG[,3],EG[,4],EG[,5],EG[,6])
  time.end=proc.time()
  print(time.end-time.start)
  for (ix in seq(nrow(EG))){
    ch<-EG[ix,1]
    bl<-EG[ix,2]
    hpm<-EG[ix,3]
    mt<-EG[ix,4]
    it<-EG[ix,5]
    result<-result_mc[[ix]]
    cu<-length(result[result!=0])
    #ms<-paste('HP miner:',hpm,'; backlog:',bl-1,'chains:',ch)
    #ss<-paste('\ncatched up:',cu,'%; iterations:',it,'max.time:',mt)
    #h<-hist(result,col='red',breaks=brk,main=ms,sub=ss,xlab='time',ylab='freq',plot=TRUE)
    h<-hist(result,breaks=brk,plot=FALSE)

    L<-c(L,ch,bl,hpm,mt,it,cu,diff(h$mids)[1],h$mids[1],list(h$counts))
  }

  fn<-paste0('catchup_2_',chains,'.rds')
  saveRDS(L,fn)
}
if (mode2a) {
  IT<-c(1000)
  MT<-c(10000)
  CH<-c(1)
  BL<-c(2)
  HPM<-c(52)
  rep<-1000
  REPEAT<-sample(100000,rep,replace=FALSE)
  chains<-CH[1]
  it<-IT[1]
  mt<-MT[1]
  ch<-CH[1]
  bl<-BL[1]
  hpm<-HPM[1]

  EG<-expand.grid(CH,BL,HPM,MT,IT,REPEAT)
  L<-c()
  result_mc<-mcmapply(Csimulate_2,EG[,1],EG[,2],EG[,3],EG[,4],EG[,5],EG[,6])
  for (ix in seq(nrow(EG))){
    ch<-EG[ix,1]
    bl<-EG[ix,2]
    hpm<-EG[ix,3]
    mt<-EG[ix,4]
    it<-EG[ix,5]
    L<-c(L,ch,bl,hpm,mt,it,list(result_mc[,ix]))}
  fn<-paste0('catchup_2a_',chains,'.rds')
  saveRDS(L,fn)
}

*/
