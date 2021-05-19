#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/skbuff.h>

int (*ra_sw_nat_hook_rx) (struct sk_buff * skb) = NULL;
int (*ra_sw_nat_hook_tx) (struct sk_buff * skb, int gmac_no) = NULL;

EXPORT_SYMBOL(ra_sw_nat_hook_rx);
EXPORT_SYMBOL(ra_sw_nat_hook_tx);

