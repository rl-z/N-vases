#include <Rcpp.h>
using namespace Rcpp;

// This is a simple example of exporting a C++ function to R. You can
// source this function into an R session using the Rcpp::sourceCpp
// function (or via the Source button on the editor toolbar). Learn
// more about Rcpp at:
//
//   http://www.rcpp.org/
//   http://adv-r.had.co.nz/Rcpp.html
//   http://gallery.rcpp.org/
//

// [[Rcpp::export]]

NumericMatrix Csimulate(int chains, int max_time,int iterations) {

  int situation_backlogs[chains];
  int pick_chain = 0;
  double miner_hp;
  double attacker_hp;
  double miner_rnd;
  double attacker_rnd;
  bool catchup_succeeded = false;
  int catchup_count = 0;
  int ix = 0;
  int iy = 0;
  NumericMatrix out(25,10);
  srand(time(NULL));

  for (int ix_backlog = 2; ix_backlog <= 11; ++ix_backlog){
    ix = 0; /* for ISO reasons */
    for (int ix_hpm = 98; ix_hpm >= 50; ix_hpm-=2) {
      miner_hp = (double) ix_hpm/100;
      attacker_hp = 1.0 - miner_hp;
      catchup_count = 0;

      for (int ix_iteration = 0; ix_iteration <iterations; ++ix_iteration) {

        for (int ixx = 0; ixx<chains; ixx++){situation_backlogs[ixx]=ix_backlog;}
        catchup_succeeded = false;

        for(int ix_time = 1; ix_time <= max_time; ++ix_time) {
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
          if (catchup_succeeded) {
            catchup_count++;
            break;}
        } /* end loop time */
      } /* end loop iteration */
      out(ix,iy) = (double) catchup_count/iterations;
      ix++;
    } /* end loop hashpower miner */
    iy++;
  } /* end loop backlog */

  return(out);
}

// You can include R code blocks in C++ files processed with sourceCpp
// (useful for testing and development). The R code will be automatically
// run after the compilation.
//

/*** R
gc()
library(parallel) # we apply a multicore solution
CH<-c(1)
chains<-CH[1]
L<-list()
IT<-c(500,1000)
MT<-c(500,1000)
EG<-expand.grid(CH,MT,IT)
N<-nrow(EG)
print(paste('chains:',chains))
time.start<-proc.time()
results<-mcmapply(Csimulate,EG[,1],EG[,2],EG[,3])
time.end<-proc.time()
time.duration<-time.end-time.start
for (ix in seq(N)){
  C<-result[[ix]]
  row.names(C)<-seq(2,50,2)
  colnames(C)<-seq(10)
  ch<-EG[ix,1]
  mt<-EG[ix,2]
  it<-EG[ix,3]
  result2<-list(ch,it,mt,C)
  L<-c(L,result2)
}
fn<-paste0('catchup_1_',chains,'.rds')
saveRDS(L,fn)

*/
