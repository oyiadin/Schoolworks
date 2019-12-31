#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>   // printk()
#include <linux/slab.h>     // kmalloc()
#include <linux/errno.h>    // error codes
#include <linux/uaccess.h>  // copy_*_user
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/sched/signal.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>

#define _cond_helper(cond, ret, inner) \
  (task->state == cond ? ret : (inner))

#define task_state(task) \
  (_cond_helper(TASK_RUNNING, "running", \
    _cond_helper(TASK_INTERRUPTIBLE, "interruptible", \
      _cond_helper(TASK_UNINTERRUPTIBLE, "uninterruptible", \
        _cond_helper(__TASK_STOPPED, "stopped", \
          _cond_helper(__TASK_TRACED, "traced", \
            _cond_helper(TASK_DEAD, "dead", \
              _cond_helper(TASK_WAKEKILL, "wakekill", \
                (task->state > 0 ? "runnable" : "unrunnable")))))))))

#define specific_mode_show_process_info(type, to, task) \
  ((task) == NULL ? 0 : (sprintf((to),  \
                                 "[" type "]\tPID=%3d, %15s, %s\n",  \
                                 (task)->pid,  \
                                 task_state(task),  \
                                 (task)->comm) + 1))

#define new_linklist_element(ptr) \
  do {  \
    ptr = kmalloc(sizeof(struct linklist), GFP_KERNEL);  \
    if (!ptr) {  \
      retval = -ENOMEM;  \
      goto out;  \
    }  \
    ptr->next = NULL;  \
  } while (0)

#define fill_element_buf(ptr) \
  do {  \
    ptr->buf = kmalloc(retval, GFP_KERNEL);  \
    ptr->bufsz = retval;  \
    if (!ptr->buf) {  \
      retval = -ENOMEM;  \
      goto out;  \
    }  \
    strncpy(ptr->buf, kbuf, retval);  \
  } while (0)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("oyiadin");


// unsigned short pid = 0;
// module_param(pid, ushort, S_IRUGO);

DEFINE_SEMAPHORE(single_instance);

struct linklist {
  char *buf;
  int bufsz;
  struct linklist *next;
};

struct linklist *head = NULL;
struct linklist *read_current_element;
int read_current_offset;


int open_handler(struct inode *node, struct file *filp) {
  char *filename = filp->f_path.dentry->d_name.name;
  char *parentname = filp->f_path.dentry->d_parent->d_name.name;

  printk(KERN_INFO "opening: [%s] => [%s]\n",
         parentname, filename);

  int retval = -EFAULT, pid = -1, show_only_threads = -1;
  struct linklist *prev = NULL, *at = NULL;
  char kbuf[512] = {0};
  struct task_struct *task = NULL, *curr_task = NULL;
  struct list_head *list;

  down_interruptible(&single_instance);
  printk(KERN_INFO "locked\n");

  if (strcmp(parentname, "pinfo") == 0) {  // traverse mode
    if (strcmp(filename, "threads") == 0) {
      show_only_threads = 1;
    } else if (strcmp(filename, "processes") == 0) {
      show_only_threads = 0;
    }

    for_each_process(task) {
      if (show_only_threads && task->mm != NULL) {
        continue;
      }
      new_linklist_element(at);
      retval = sprintf(kbuf,
                       "[%s] %s\n"
                       "     PID: %d\n"
                       "   State: %s\n"
                       "Priority: %d\n"
                       "  Parent: %d\n\n",
                       (show_only_threads ? "Threads" : "Process"),
                       task->comm,
                       task->pid,
                       task_state(task),
                       task->prio-100,
                       task->parent->pid) + 1;  // for NULL
      fill_element_buf(at);

      if (!prev) {  // the first element
        head = at;
        prev = at;
      } else {
        prev->next = at;
        prev = at;
      }
    }

  } else if (strcmp(parentname, "pid") == 0) {  // specific process mode
    retval = kstrtoint(filename, 10, &pid);
    if (retval < 0)
      goto out;

    struct pid *pid_struct = find_get_pid(pid);
    if (!pid_struct) {
      retval = -ESRCH;
      goto out;
    }
    task = pid_task(pid_struct, PIDTYPE_PID);
    if (!task) {
      retval = -ESRCH;
      goto out;
    }
    printk(KERN_INFO "task found\n");


    new_linklist_element(at);
    retval = specific_mode_show_process_info("Myself", kbuf, task);
    fill_element_buf(at);
    head = at;
    prev = at;
    // printk(KERN_INFO "self printed to buffer\n");

    curr_task = task->parent;
    new_linklist_element(at);
    retval = specific_mode_show_process_info("Parent", kbuf, curr_task);
    fill_element_buf(at);
    prev->next = at;
    prev = at;
    // printk(KERN_INFO "parent printed to buffer\n");

    list_for_each(list, &task->parent->children) {
      curr_task = list_entry(list, struct task_struct, sibling);
      new_linklist_element(at);
      retval = specific_mode_show_process_info("Sibling", kbuf, curr_task);
      fill_element_buf(at);
      prev->next = at;
      prev = at;
      // printk(KERN_INFO "sibling appended\n");
    }

    list_for_each(list, &task->children) {
      curr_task = list_entry(list, struct task_struct, sibling);
      new_linklist_element(at);
      retval = specific_mode_show_process_info("Children", kbuf, curr_task);
      fill_element_buf(at);
      prev->next = at;
      prev = at;
      // printk(KERN_INFO "child appended\n");
    }
  }

  read_current_element = head;
  read_current_offset = 0;

  retval = 0;

out:
  if (retval) {
    printk(KERN_INFO "unlocked in cleanup\n");
    up(&single_instance);
  }
  return retval;
}

int release_handler(struct inode *node, struct file *filp) {
  struct task_struct *task;
  struct linklist *at = head, *prev;

  while (at) {
    prev = at;
    at = at->next;
    kfree(prev->buf);
    kfree(prev);
  }
  // printk(KERN_INFO "buffer freed\n");

  up(&single_instance);
  // printk(KERN_INFO "unlocked in release\n");
}


ssize_t read_handler(
    struct file *filp,
    char __user *buf,
    size_t sz,
    loff_t *pos) {

  // printk(KERN_INFO "read\n");

  int retval = -EFAULT;

  if (sz < 1) {
    printk(KERN_WARNING "read(): size %ld out of range\n", sz);
    retval = -EINVAL;
    goto out;
  }

  if (!read_current_element) {
    retval = 0;  // EOF
    goto out;
  }

  int remaining_sz = read_current_element->bufsz-1 - read_current_offset;
  if (remaining_sz > sz) {
    // if the requested size is smaller than "one process"
    retval = copy_to_user(buf, read_current_element->buf + read_current_offset, sz);
    if (retval == sz) {
      // if no any data was transferred
      retval = -EADDRNOTAVAIL;
      goto out;
    }
    read_current_offset += sz - retval;
    retval = sz - retval;

  } else {
    // if the requested size is larger than "one process"
    retval = copy_to_user(
      buf,
      read_current_element->buf + read_current_offset,
      remaining_sz);
    if (retval == remaining_sz) {
      // if no any data was transferred
      retval = -EADDRNOTAVAIL;
      goto out;
    }
    read_current_offset = 0;
    read_current_element = read_current_element->next;
    retval = remaining_sz;
  }

out:
  return retval;
}


static const struct file_operations file_ops = {
  .owner = THIS_MODULE,
  .open  = open_handler,
  .release = release_handler,
  .read  = read_handler,
};


#define proc_mkdir_chk(to, name, parent) \
  do {  \
    to = proc_mkdir((name), (parent));  \
    if (to == NULL)  \
      return -ENOMEM;  \
  } while (0)

#define proc_create_chk(to, name, parent) \
  do {  \
    to = proc_create((name), 0, (parent), &file_ops);  \
    if (to == NULL)  \
      return -ENOMEM;  \
  } while (0)

static int __init pinfo_init(void) {
  int i;
  struct proc_dir_entry *dir_pinfo, *dir_pid, *proc;

  proc_mkdir_chk(dir_pinfo, "pinfo", NULL);
  proc_mkdir_chk(dir_pid, "pid", dir_pinfo);

  proc_create_chk(proc, "threads", dir_pinfo);
  proc_create_chk(proc, "processes", dir_pinfo);

  char buf[5] = {0};
  for (i=0; i<1024; ++i) {
    snprintf(buf, 5, "%d", i);
    proc_create_chk(proc, buf, dir_pid);
  }

  printk(KERN_INFO "proc entries initialized\n");

  return 0;
}


static void __exit pinfo_cleanup(void) {
  remove_proc_subtree("pinfo", NULL);
  // remove_proc_entry("pinfo", NULL);
}


module_init(pinfo_init);
module_exit(pinfo_cleanup);

