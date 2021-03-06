package cn.edm.utils;

import java.io.File;
import java.util.Collection;
import java.util.Map;

import org.apache.commons.lang.StringUtils;
import org.springframework.web.multipart.MultipartFile;

import cn.edm.exception.Errors;

public class Asserts {

	public static void isNull(Object obj, String message) {
        if (obj == null)
            throw new Errors(message + "不存在");
	}

    public static void notUnique(boolean unique, String message) {
        if (!unique)
            throw new Errors(message + "已存在");
    }
	
	public static void notExists(String savePath, String filePath, String message) {
		File file = new File(savePath + filePath);
		if (!file.exists()) {
			throw new Errors(message + "文件不存在");
		}
	}
	
	public static void isEmpty(Map<?, ?> map, String message) {
		if (empty(map)) {
			throw new Errors(message + "集合不能为空值");
		}
	}
	
	public static void isEmpty(Collection<?> list, String message) {
		if (empty(list)) {
			throw new Errors(message + "集合不能为空值");
		}
	}
	
	public static void isEmpty(Object[] objects, String message) {
		if (empty(objects)) {
			throw new Errors(message + "集合不能为空值");
		}
	}
	
	public static void notEquals(Object[] objects, int length, String message) {
		if (empty(objects) || objects.length != length) {
			throw new Errors(message + "集合元素不等于" + length + "个");
		}
	}
	
    public static void password(String password, String message) {
        if (StringUtils.isNotBlank(password)) {
            boolean az = false;
            boolean _09 = false;
            boolean _ = false;
            for (char c : password.toCharArray()) {
                if (((int) c >= 97 && (int) c <= 122) || ((int) c >= 65 && (int) c <= 90)) {
                    az = true;
                }
                if ((int) c >= 48 && (int) c <= 57) {
                    _09 = true;
                }
                if ((int) c == 95) {
                    _ = true;
                }
            }
            if (!(az && _09 && _)) {
                throw new Errors(message + "必须由字母、数字和下划线组成");
            }
        }
    }
    /**
	 * 验证密码
	 * 只能由字母、数字或下划线组成
	 * @param password
	 * @param message
	 * @return
	 */
    public static String validatePwd(String password, String message) {
        if (StringUtils.isNotBlank(password)) {
            boolean az = false;
            boolean _09 = false;
            boolean _ = false;
            for (char c : password.toCharArray()) {
                if (((int) c >= 97 && (int) c <= 122) || ((int) c >= 65 && (int) c <= 90)) {
                    az = true;
                }
                if ((int) c >= 48 && (int) c <= 57) {
                    _09 = true;
                }
                if ((int) c == 95) {
                    _ = true;
                }
            }
            if (!(az && _09 && _)) {
                return message + "只能由字母、数字或下划线组成";
            }
        }
        return null;
    }
    
	public static boolean empty(Map<?, ?> map) {
		if (map == null || map.isEmpty()) {
			return true;
		}
		return false;
	}

	public static boolean empty(Collection<?> list) {
		if (list == null || list.isEmpty()) {
			return true;
		}
		return false;
	}

	public static boolean empty(Object[] objects) {
		if (objects == null || objects.length == 0) {
			return true;
		}
		return false;
	}
	
	public static boolean empty(MultipartFile file) {
		if (file == null || file.isEmpty()) {
			return true;
		}
		return false;
	}
	
	public static boolean hasAny(Object target, Object[] objects) {
		if (objects == null || objects.length == 0) {
			return false;
		}
		if (target == null) {
			return false;
		}

		for (Object object : objects) {
			if (object.equals(target)) {
				return true;
			}
		}
		
		return false;
	}
	
	public static void notHasAny(Object target, Object[] objects, String message) {
		if (!hasAny(target, objects)) {
			throw new Errors(message);
		}
	}
	
	public static boolean containsAny(String target, String[] objects) {
		if (objects == null || objects.length == 0) {
			return false;
		}
		if (target == null) {
			return false;
		}

		for (String object : objects) {
			if (StringUtils.contains(target, object)) {
				return true;
			}
		}

		return false;
	}
	
	public static boolean equals(Object[] objects1, Object[] objects2) {
		boolean left = true;
		boolean right = true;
		
		if (!empty(objects2)) {
			for (Object object : objects2) {
				if (!Asserts.hasAny(object, objects1)) {
					left = false;
					break;
				}
			}
		}
		
		if (!empty(objects1)) {
			for (Object object : objects1) {
				if (!Asserts.hasAny(object, objects2)) {
					right = false;
					break;
				}
			}
		}

		return left && right;
	}

}
