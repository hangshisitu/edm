package cn.edm.utils;

import java.math.BigDecimal;
/**
 * 运算工具类
 * 
 * @Date 2014年6月14日 下午5:56:33
 * @author Lich
 *
 */
public class ArithUtil {
	
	private final static Integer DEFAULT_SCALE = 2;
	
    /**
     * 提供精确的加法运算。
     * @param d1
     * @param d2
     * @return double
     */
    public static double add(double d1, double d2) { // 进行加法运算
		BigDecimal b1 = new BigDecimal(d1);
		BigDecimal b2 = new BigDecimal(d2);
		return b1.add(b2).doubleValue();
	}
    /**
     * 提供精确的减法运算。
     *
     * @param minuend   被减数
     * @param subtrahend  减数
     * @return double
     */
    public static double sub(double minuend, double subtrahend) {
		BigDecimal b1 = new BigDecimal(minuend);
		BigDecimal b2 = new BigDecimal(subtrahend);
		return b1.subtract(b2).doubleValue();
	}
    /**
     * 提供精确的乘法运算。
     *
     * @param multiplicator 乘数
     * @param multiplicand  被乘数
     * @return double
     */
	public static double mul(double multiplicator, double multiplicand) {
		BigDecimal b1 = new BigDecimal(multiplicator);
		BigDecimal b2 = new BigDecimal(multiplicand);
		return b1.multiply(b2).doubleValue();
	}
	/**
	 * 提供精确的除法运算。
	 * @param divider 除数    不能为0
	 * @param dividend  被除数
	 * @return double
	 */
	public static double div(double divider, double dividend) {
		return div(divider, dividend, DEFAULT_SCALE);
	}
    /**
     * 提供精确的除法运算。
     * @param divider 除数    不能为0
     * @param dividend  被除数
     * @param scale         精度，BigDecimal.ROUND_UP 向上保留小数，即2.123保留成2.13，2.128也保留成2.13
     * @return double
     */
	public static double div(double divider, double dividend, int scale) {
		BigDecimal b1 = new BigDecimal(divider);
		BigDecimal b2 = new BigDecimal(dividend);
		return b1.divide(b2, scale, BigDecimal.ROUND_HALF_UP).doubleValue();
	}
	/**
	 * 进行四舍五入操作
	 * @param d
	 * @param len
	 * @return double
	 */
	public static double round(double d, int len) {
		BigDecimal b1 = new BigDecimal(d);
		BigDecimal b2 = new BigDecimal(1);
		// 任何一个数字除以1都是原数字
		// ROUND_HALF_UP是BigDecimal的一个常量， 表示进行四舍五入的操作
		return b1.divide(b2, len, BigDecimal.ROUND_HALF_UP).doubleValue();
	}
	
    public static void main(String[] args) {
    	Double db = ArithUtil.div(1, 9, 2);
    	System.out.println(db);
	}
}
