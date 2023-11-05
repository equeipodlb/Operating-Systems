
//COMO USAR EL DISPOSITIVO
//1: Cargar el modulo con (con sudo insmod chardev_leds.ko) (Primero compilamos todo con make)
//2: Para ver el major del modulo, usamos cat /proc/devices y como minor se asigna al 0 o uno mayor si ya está el 0 asignado a un dispositivo de este tipo
//3: Creamos el dispositivo de carácteres con el comando sudo mknod /dev/led -m 666 c Major Minor
//4: Hacemos sudo echo 123 > /dev/led para escribir en el dispositivo de carácteres
//5: Hacemos sudo cat /dev/led para leer el archivo de carácteres
//6: Descargamos el módulo con sudo rmmod chardev_leds.ko

//Para chequear el log del sistema usamos sudo dmesg y para ver los módulos actualmente cargados usamos lsmod

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/tty.h>      /* For fg_console */
#include <linux/kd.h>       /* For KDSETLED */
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/vt_kern.h>
#include <linux/version.h>  /* For LINUX_VERSION_CODE */

#define DEVICE_NAME "chardev_leds"
#define NUM_LOCK_LED_NAME "num_lock_led"
#define CAPS_LOCK_LED_NAME "caps_lock_led"
#define SCROLL_LOCK_LED_NAME "scroll_lock_led"

#define LED_SCROLL_ON 0x1 //corresponde a la secuencia 001
#define LED_NUM_ON 0x2 //corresponde a la secuencia 010
#define LED_CAPS_ON 0x4 //corresponde a la secuencia 100
#define ALL_LEDS_OFF 0x0 //corresponde a la secuencia 000

static struct cdev chardev_leds_cdev;
struct tty_driver* kbd_driver= NULL;
int active_leds;

/* Prototipos de funciones */
static int chardev_leds_open(struct inode *inode, struct file *file);
static int chardev_leds_release(struct inode *inode, struct file *file);
static ssize_t chardev_leds_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos);
static ssize_t device_read(struct file *filp,  char *buff, size_t len, loff_t * off);

struct tty_driver* get_kbd_driver_handler(void) //Se invoca durante la carga del modulo para obtener un puntero al manejador del drivel del teclado
{
    printk(KERN_INFO "chardev_leds: loading\n");
    printk(KERN_INFO "chardev_leds: fgconsole is %x\n", fg_console);
#if ( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,32) )
    return vc_cons[fg_console].d->port.tty->driver;
#else
    return vc_cons[fg_console].d->vc_tty->driver;
#endif
}
/* Set led state to that specified by mask */
//Permite establecer el valor de los leds. Acepta como parámetros un puntero al manejador del driver y una máscara
// de bits que específica el estado de cada led (1 ON y 0 OFF). Cada bit controla un led específico del teclado, el bit 0 el scroll lock, el bit1 el numLock, el bit 3 el 
//capsLock y los bits 4 y 5 se ignoran
static inline int set_leds(struct tty_driver* handler, unsigned int mask)
{
#if ( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,32) )
    return (handler->ops->ioctl) (vc_cons[fg_console].d->port.tty, KDSETLED,mask);
#else
    return (handler->ops->ioctl) (vc_cons[fg_console].d->vc_tty, NULL, KDSETLED, mask);
#endif
}

/* Estructura de operaciones de archivos */
static const struct file_operations chardev_leds_fops = {
    .owner = THIS_MODULE,
    .read = device_read,
    .open = chardev_leds_open,
    .release = chardev_leds_release,
    .write = chardev_leds_write,
};

/* Función de inicialización del módulo */ //Función a la que se llama al cargar el modulo con (con sudo insmod chardev_leds)
static int __init chardev_leds_init(void)
{
    int ret;
    dev_t dev_id;
    kbd_driver=get_kbd_driver_handler();

    /* Reservar número de dispositivo */
    ret = alloc_chrdev_region(&dev_id, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        pr_err("Error al reservar el número de dispositivo\n");
        return ret;
    }

    //Hacemos las dos siguientes acciones para que el módulo se registre a si mismo en el sistema como un driver de dispositivo de carácteres

    /* Inicializar el dispositivo de caracteres */
    cdev_init(&chardev_leds_cdev, &chardev_leds_fops);
    chardev_leds_cdev.owner = THIS_MODULE;

    /* Añadir el dispositivo de caracteres al sistema */
    ret = cdev_add(&chardev_leds_cdev, dev_id, 1);
    if (ret < 0) {
        pr_err("Error al añadir el dispositivo de caracteres\n");
        unregister_chrdev_region(dev_id, 1);
        return ret;
    }
    return 0;
}

/* Función de salida del módulo */
static void __exit chardev_leds_exit(void) //Función a la que se llama al descargar el modulo (con sudo rmmod chardev_leds)
{
    dev_t dev_id;

    /* Desvincular el dispositivo de caracteres del sistema */
    cdev_del(&chardev_leds_cdev);

    /* Liberar el número de dispositivo */
    dev_id = chardev_leds_cdev.dev;
    unregister_chrdev_region(dev_id, 1);
}

//TENER EN CUENTA QUE TANTO CAT COMO ECHO ABREN, ESCRIBEN O LEEN Y CIERRAN EL FICHERO DE DISPOSITIVOS, POR TANTO LLAMAN A LAS 3 FUNCIONES

/* Función de apertura del dispositivo de carácteres */ 
static int chardev_leds_open(struct inode *inode, struct file *file)
{
    //El encendido de leds se realiza al escribir en el dispositivo de caracteres, en la funcion write
    return 0;
    
}

/* Función de cierre del dispositivo de carácteres */ 
static int chardev_leds_release(struct inode *inode, struct file *file) {
    return 0;//No hacemos nada al cerrar el dispositivo de carácteres
}

/* Función de escritura del dispositivo */ 
//Se la llama tras realizar sudo echo 123 > /dev/led ya que con este comando escribimos en el dispositivo de carácteres. 
static ssize_t chardev_leds_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    char command[count + 1];
    int i;
    
    /* Leer la cadena de comandos desde el buffer */
    if (copy_from_user(command, buf, count)) //Hacemos siempre copy_from_user desde buf a una variable locla privada en lugar de usar directamente el buffer para evitar datos corrompidos
        return -EFAULT;
    command[count] = '\0';

    /* Procesar cada comando de la cadena */
    active_leds = ALL_LEDS_OFF; //Para empezar apagamos todos los leds
    for (i = 0; i < count; i++) {
        if (command[i] == '1'){
            active_leds |= LED_NUM_ON;
            printk("Se ha alcanzado el 1\n");
        }
        else if (command[i] == '2'){
            active_leds |= LED_CAPS_ON;
            printk("Se ha alcanzado el 2\n");
        }
        else if (command[i] == '3'){       
            active_leds |= LED_SCROLL_ON;
            printk("Se ha alcanzado el 3\n");
        }
    }
    // Una vez sabemos que leds activar, los encendemos
    set_leds(kbd_driver,active_leds);
    return count;
}
static ssize_t
device_read(struct file *filp,  char *buff, size_t len, loff_t * off)
{
    printk(KERN_ALERT "Sorry, this operation isn't supported.\n");
    return -EPERM;
}
module_init(chardev_leds_init);
module_exit(chardev_leds_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Character Device Driver for Keyboard LEDs");


