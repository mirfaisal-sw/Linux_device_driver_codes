/*******************************************************
 * Character Device Driver Template Code
 *******************************************************/

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <asm/uaccess.h>

#define FIRST_MINOR 0
#define MINOR_CNT 1

struct uart_omap_port {
	struct uart_port	port;
	struct device		*dev;

	unsigned char		ier;
	unsigned char		lcr;
	unsigned char		mcr;
	unsigned char		fcr;
	unsigned char		efr;
	unsigned char		dll;
	unsigned char		dlh;
	unsigned char		mdr1;
	unsigned char		scr;
	unsigned char		wer;

	char *rx_buff;
	size_t rx_size;
	char *tx_buff;
	size_t tx_size;

	dev_t devt;
	struct cdev c_dev;
	struct class *cl;
};

static int my_open(struct inode *i, struct file *f)
{
	return 0;  
}
static int my_close(struct inode *i, struct file *f)
{
	return 0;
}

static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{

	return len;
}
static ssize_t my_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
		
	return len;		
}

static struct file_operations driver_fops =
{
	.owner = THIS_MODULE,
	.open = my_open,
	.release = my_close,
	.read = my_read,
	.write = my_write
};

int my_init(struct uart_omap_port *up)
{
	int ret;
	struct device *dev_ret;
	omap_port = up;

	if ((ret = alloc_chrdev_region(&up->devt, FIRST_MINOR, MINOR_CNT, "uart_driver")) < 0)
	{
		return ret;
	}

	cdev_init(&up->c_dev, &driver_fops);

	if ((ret = cdev_add(&up->c_dev, up->devt, MINOR_CNT)) < 0)
	{
		unregister_chrdev_region(up->devt, MINOR_CNT);
		return ret;
	}
	
	if (IS_ERR(up->cl = class_create(THIS_MODULE, "uart")))
	{
		cdev_del(&up->c_dev);
		unregister_chrdev_region(up->devt, MINOR_CNT);
		return PTR_ERR(up->cl);
	}
	if (IS_ERR(dev_ret = device_create(up->cl, NULL, up->devt, NULL, "uart%d", FIRST_MINOR)))
	{
		class_destroy(up->cl);
		cdev_del(&up->c_dev);
		unregister_chrdev_region(up->devt, MINOR_CNT);
		return PTR_ERR(dev_ret);
	}

	return 0;
}

void tga_uart_exit(void)
{
	device_destroy(omap_port->cl, omap_port->devt);
	class_destroy(omap_port->cl);
	cdev_del(&omap_port->c_dev);
	unregister_chrdev_region(omap_port->devt, MINOR_CNT);
	serial_omap_shutdown(&omap_port->port);
}




