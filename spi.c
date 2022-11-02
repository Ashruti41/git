#include<linux/spi/spi.h>
#include<linux/spi/spi.c>

#include<linux/module.h>


struct spi_device {
	struct devicedev;
	struct spi_master *master;
	u32 max_speed_hz;
	u8 chip_select;
	u8 bits_per_word;
	u16 mode;
	int irq;
	int cs_gpio;
};

struct spi_device_id {
	char name[SPI_NAME_SIZE];
	kernel_ulong_t driver_data; /* data private to the driver */
};

static int my_probe(struct spi_device *spi)
{
	/*bits per word cannot be configure in platform data */
	spi->mode = SPI_MODE_0; /*CPHA=0, CPOL=0 */
	spi->max_speed_hz = 20000000; /*Max clock for the device */
	spi->bits_per_word = 16;/*device bits per word */
	ret = spi_setup(spi);
	ret = spi_setup(spi);
	if(ret < 0)
		return ret;

	return ret;
}
/* mc33880 example */
struct mc33880 {
	struct mutex lock;
	u8 bar;
	struct foo chip;
	struct spi_device *spi;
};

static int mc33880_probe(struct spi_device *spi)
{
	struct mc33880 *mc;

	mc = devm_kzalloc(&spi->dev, sizeof(struct mc33880), GFP_KERNEL);

	if(!mc)
		return -ENOMEM;

	mutex_init(&mc->lock);
	spi_set_drvdata(spi,mc);

	mc->spi = spi;
	mc->chip.lable = DRIVER_NAME;
	mc->chip.set = mc33880_set;
}
static int mc33880_remove(struct spi_device *spi)
{
	struct mc33880 *mc;
	mc = spi_get_drvdata(spi);
	if(!mc)
		return -ENODEV;

	mutex_destroy(&mc->lock);
	return 0;
}

//driver initalization
static int __init foo_init(void)
{
	return spi_register_driver(&foo_driver);
}
module_init(foo_init);
static void __exit foo_cleanup(void)
{
	return spi_unregister_driver(&foo_driver);
}
module_exit(foo_cleanup);

static struct spi_device_id foo_idtable[] = {
	{"foo", ID_FOR_FOO_DEVICE },
	{"bar", ID_FOR_BAR_DEVICE },
	{ }
};
MODULE_DEVICE_TABLE(spi, foo_idtable);

static struct spi_driver foo_driver = {
	.driver = {
	.name = "KBUILD_MODULE",
	},

	.id_table = foo_idtable,
	.probe = foo_probe,
	.remove = foo_remove,
};
module_spi_driver(foo_driver);



