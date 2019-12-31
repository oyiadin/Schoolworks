#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>   // printk()
#include <linux/slab.h>     // kmalloc()
#include <linux/errno.h>    // error codes
#include <linux/uaccess.h>  // copy_*_user
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/semaphore.h>
#include <linux/types.h>
#include <linux/cdev.h>


// #define min(a, b) ((a < b) ? (a) : (b))
#define KMALLOC_CHECK(PTR, LABEL) \
  do { \
    if (PTR == NULL) { \
      printk(KERN_ERR "cannot kmalloc(sz=%lu)\n", sz); \
      retval = -EFAULT; \
      goto LABEL; \
    } \
  } while (0)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("oyiadin");


typedef struct mem_block {
  void *mem;
  size_t sz;
  struct mem_block *next;
} mem_block;


dev_t dev;
struct cdev cdev;

int unhandled_read_errno = 0;

mem_block *head = NULL,
          *foot = NULL;

DEFINE_SEMAPHORE(lock_mem);
DEFINE_SEMAPHORE(lock_errno);

void clear(void) {
  mem_block *at = head, *next = NULL;
  while (at) {
    next = at->next;
    kfree(at->mem);
    kfree(at);
    at = next;
  }
  head = foot = NULL;
}


ssize_t my_os_exp_chrdev_read(struct file *filp, char __user *buf, size_t sz, loff_t *pos) {

  int retval = -1;
  size_t transferred = 0;
  mem_block *new_block = NULL;

  printk(KERN_INFO "read(filp=%p, buf=%p, sz=%lu, pos=%lld\n", filp, buf, sz, *pos);

  down(&lock_errno);
  if (unhandled_read_errno) {
    retval = unhandled_read_errno;
    unhandled_read_errno = 0;
    up(&lock_errno);
    goto out;
  }
  up(&lock_errno);

  if (sz < 1) {
    printk(KERN_WARNING "read(): size %ld out of range\n", sz);
    retval = -EINVAL;
    goto out;
  }

  down(&lock_mem);
  if (head == NULL) {
    retval = 0;  // EOF
    up(&lock_mem);

  } else {
    transferred = min(sz, head->sz)
                  - copy_to_user(buf,
                                 head->mem,
                                 min(sz, head->sz));
    if (transferred == 0) {
      retval = -EFAULT;
      goto unlock_mem;
    }

    if (head->sz > transferred) {  // need to split into two blocks
      mem_block *new_block = (mem_block *) kmalloc(sizeof(mem_block), GFP_KERNEL);
      char *new_mem = (char *) kmalloc(head->sz - transferred, GFP_KERNEL);
      KMALLOC_CHECK(new_block, unlock_mem);
      KMALLOC_CHECK(new_mem, unlock_mem);

      new_block->mem = new_mem;
      new_block->sz = head->sz - transferred;
      new_block->next = head->next;
      memcpy(
        new_block->mem,
        head->mem + transferred,
        head->sz - transferred);

      kfree(head->mem);
      kfree(head);
      if (foot == head)
        foot = new_block;
      head = new_block;

      up(&lock_mem);

    } else /* if (head->sz == transferred) */ {
      mem_block *next = head->next;
      kfree(head->mem);
      kfree(head);
      if (foot == head)
        foot = NULL;
      head = next;

      up(&lock_mem);
    }

    retval = transferred;
  }

  goto out;

 unlock_mem:
  up(&lock_mem);
 out:
  return retval;
}


ssize_t my_os_exp_chrdev_write(struct file *filp, const char __user *buf, size_t sz, loff_t *pos) {

  int retval = -1;
  void *kbuf = NULL;
  mem_block *new_block = NULL;

  printk(KERN_INFO "write(filp=%p, buf=%p, sz=%lu, pos=%lld\n", filp, buf, sz, *pos);

  if (sz < 1) {
    printk(KERN_WARNING "write(): size %ld out of range\n", sz);
    retval = -EINVAL;
    goto out;
  }

  kbuf = kmalloc(sz, GFP_KERNEL);
  KMALLOC_CHECK(kbuf, out);

  retval = copy_from_user(kbuf, buf, sz);
  // returns how many bytes failed to transfer

  if (retval > 0) {  // transfer failed
    if (sz == retval) {  // no any bytes have been transfered
      // the user's buffer pointer may be illegal
      // we won't handle it any more, return immediately.
      printk(KERN_ERR "no any bytes have been transfered, return immediately\n");
      retval = -EFAULT;
      goto free_kbuf;

    } else {  // partially tranfered some data
      printk(KERN_NOTICE "partially tranfered some data\n");
      retval = sz - retval;  // bytes transfered
      unhandled_read_errno = -EFAULT;
      // will be dealt with in the next read() call
      goto free_kbuf;
    }
  }

  retval = sz;

  new_block = kmalloc(sizeof(mem_block), GFP_KERNEL);
  KMALLOC_CHECK(new_block, free_kbuf);

  new_block->mem = kbuf;
  new_block->sz = sz;
  new_block->next = NULL;

  down(&lock_mem);
  if (foot) {
    foot->next = new_block;
    foot = new_block;
  } else {
    head = foot = new_block;
  }
  up(&lock_mem);

  goto out;

 free_kbuf:
  kfree(kbuf);
 out:
  return retval;
}


int my_os_exp_chrdev_open(struct inode *inode, struct file *filp) {
  printk(KERN_INFO "open(inode=%p, filp=%p)\n", inode, filp);
  return 0;
}


int my_os_exp_chrdev_release(struct inode *inode, struct file *filp) {
  printk(KERN_INFO "release(inode=%p, filp=%p)\n", inode, filp);
  clear();
  return 0;
}


long my_os_exp_chrdev_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) {
  switch (cmd) {
    case 0xdeadbeef:
      clear();
      break;

    default:
      printk(KERN_ERR "unknown ioctl command: %d\n", cmd);
  }

  return 0;
}


struct file_operations my_os_exp_chrdev_fops = {
  .owner = THIS_MODULE,
  .open = my_os_exp_chrdev_open,
  .read = my_os_exp_chrdev_read,
  .write = my_os_exp_chrdev_write,
  .release = my_os_exp_chrdev_release,
  .unlocked_ioctl = my_os_exp_chrdev_ioctl
};


static int __init my_os_exp_chrdev_init(void) {
  int retval = -1;

  retval = alloc_chrdev_region(&dev, 0, 1, "my_os_exp_chrdev");
  if (retval < 0) {
    printk(KERN_ERR "cannot get a device number\n");
    goto out;
  } else {
    printk(KERN_INFO "got a device number %d\n", MAJOR(dev));
  }

  cdev_init(&cdev, &my_os_exp_chrdev_fops);
  cdev.owner = THIS_MODULE;

  retval = cdev_add(&cdev, dev, 1);
  if (retval < 0) {
    printk(KERN_ERR "oops, cdev_add() failed\n");
    unregister_chrdev_region(dev, 1);
    goto out;
  }

  retval = 0;

 out:
  return retval;
}


static void __exit my_os_exp_chrdev_cleanup(void) {
  cdev_del(&cdev);
  unregister_chrdev_region(dev, 1);
  printk(KERN_INFO "goodbye\n");
}


module_init(my_os_exp_chrdev_init);
module_exit(my_os_exp_chrdev_cleanup);

