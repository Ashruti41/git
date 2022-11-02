#include<linux/module.h>
#include<linux/i2c.h>
#include<linux/string.h>
#include<linux/type.h>
#include<linux/of.h>
#include<linux/of_device.h>
#include<media/v4l2-ctrls.h>
#include<media/v4l2-device.h>

#define to_sensor_device(sub_dev) container_of(sub_dev, struct sensor_device, sd)

static int sensor_i2c_read_reg(struct v4l2_subdev *sd, u16 reg, u16 len, void *val)
{
	struct sensor_device *dev = to_sensor_device(sd);
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int ret;

	if(len == SENSOR_REG16)
		ret = regmap_read(dev->regmap16, reg, val);
	else
		ret = -EINVAL;
	if(ret) {
		dev_err(&client->dev, "Read reg failed, Reg=0x%04x, ret=%d",reg, ret);
		return ret;
	}

	if(len == SENSOR_REG16)
		dev_dbg(&client->dev, "read_reg[0x%04x] = 0x%04x\n", reg, *(u16 *)val);
	return ret;
}

static int sensor_i2c_write_reg(struct v4l2_subdev *sd, u16 reg, u16 len,u32 val)
{
	struct sensor_device *dev = to_sensor_device(sd);
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int ret;

	if(len == SENSOR_REG16)
		ret = regmap_write(dev->regmap16, reg, val);
	else
		ret = -EINVAL;
	if(ret) {
		dev_err(&client->dev, "%s: Write reg failed, Reg=0x%04x, ret=%d", __func__, reg, ret);
		return ret;
	}

	if(len == SENSOR_REG16)
		dev_dbg(&client->dev, "read_reg[0x%04x] = 0x%04x\n", reg, *(u16 *)val);
	return ret;
}

static int sensor_reg(struct v4l2_subdev *sd)
{
	struct sensor_device *dev = to_sensor_device(sd);
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int ret;
	u16 reg_val;

	ret = sensor_i2c_write_reg(sd, 0x10, SENSOR_REG16, 0x032)
	if(ret){
		dev_err(&client->dev, "%s: Failed to write 0x10 register %d\n", __func__, ret);
		return ret;
	}

	ret = sensor_i2c_read(sd, 0x12, SENSOR_REG16, &reg_val);
	if(ret){
		dev_err(&client->dev, "%s: Failed to read 0x12 register %d\n", __func__, ret);
		return ret;
	}
	dev_info(&client->dev, "%s: the reg_val = %x\n", reg_val);

	ret = sensor_i2c_read(sd, 0x14, SENSOR_REG16, &reg_val);
	if(ret){
		dev_err(&client->dev, "%s: Failed to read 0x14 register %d\n", __func__, ret);
		return ret;
	}
	dev_info(&client->dev, "%s: the reg_val = %x\n", reg_val);

	ret = sensor_i2c_write_reg(sd, 0x16, SENSOR_REG16, 0x032)
	if(ret){
		dev_err(&client->dev, "%s: Failed to write 0x16 register %d\n", __func__, ret);
		return ret;
	}

static int i2c_sensor_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct sensor_device *dev;
	int ret;
	unsigned int i;

	dev_info(&client->dev, "probe called\n");
	
	/*allocate device & init sub device*/
	dev = devm_kzalloc(&client->dev, sizeof(*dev), GFP_KERNEL);
	if (!dev) {
		dev_err(&client->dev, "%s: out of memory\n", __func__);
		return -ENOMEM;
	}

	dev->regmap16 = devm_regmap_init_i2c(client, &ap1302_reg16_config);

	if (IS_ERR(dev->regmap16)) {
		ret = PTR_ERR(dev->regmap16);
		dev_err(&client->dev,
			"Failed to allocate 16bit register map: %d\n", ret);
		goto out_free;
	}
	
	ret = sensor_reg(&dev->sd);
	if (ret) {
		dev_err(&client->dev, "Cannot read sensor register.\n");
		ret = 0;
	}

	dev_info(&client->dev, "Probe finished successfully: %d\n", ret);
	return ret;

out_free:
	dev_err(&client->dev, "Probe failed: %d\n", ret);
	v4l2_device_unregister_subdev(&dev->sd);
	return ret;
}

//i2c slave id supported by driver
static const struct i2c_device_id sensor_id[] = {
	{ SENSOR_DRV_NAME, 0},
	{ },
};
MODULE_DEVICE_TABLE(i2c, sensor_id);


static const struct of_device_id sensor_dt_ids[] = {
	{ .compatible = "sensor",}, //COMPATIBLE node must match dts
	{/*sentinel*/},
};
MODULE_DEVICE_TABLE(of, sensor_dt_ids);

//i2c driver information
static struct i2c_driver sensor_driver = {
	.driver = {
		.owner = THIS_MODULE,
		.name = SENSOR_DRV_NAME;
		.of_match_Table = sensor_dt_ids,
	},
	.probe = i2c_sensor_probe,
	.id_table = sensor_id,
};

static int __init simple_init(void)
{
	int ret;

	ret = i2c_add_driver(&sensor_driver);
	if(ret) {
		pr_err("simple driver initialization failed\n");
		return ret;
	}
	return 0;		
}

static void __exit simple_exit(void)
{
	i2c_del_driver(&sensor_driver);
}

module_init(sensor_init);
module_exit(sensor_exit);

MODULE_DESCRIPTION("PRACTICE DRIVER");
MODULE_AUTHOR("ASHRUTI");
MODULE_LICENSE("GPL");
:	
	

