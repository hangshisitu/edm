package cn.edm.util;

import java.io.UnsupportedEncodingException;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

import org.apache.commons.codec.binary.Hex;



public class Md5PasswordEncoder{

		private String defaultSalt;

		public String getDefaultSalt() {
			return defaultSalt;
		}

		public void setSefaultSalt(String defaultSalt) {
			this.defaultSalt = defaultSalt;
		}
		
		public String encodePassword(String rawPwd) {
			return encodePassword(rawPwd, defaultSalt);
		}

		public String encodePassword(String rawPwd, String salt) {
			String saltedPass = mergePasswordAndSalt(rawPwd, salt, false);
			MessageDigest messageDigest = getMessageDigest();
			byte[] digest;
			try {
				digest = messageDigest.digest(saltedPass.getBytes("UTF-8"));
			} catch (UnsupportedEncodingException e) {
				throw new IllegalStateException("UTF-8 not supported!");
			}
			return new String(Hex.encodeHex(digest));
		}

		public boolean isPasswordValid(String encPwd, String rawPwd) {
			return isPasswordValid(encPwd, rawPwd, defaultSalt);
		}
		
		public boolean isPasswordValid(String encPwd, String rawPwd, String salt) {
			if (encPwd == null) {
				return false;
			}
			String pass2 = encodePassword(rawPwd, salt);
			return encPwd.equals(pass2);
		}

		protected final MessageDigest getMessageDigest() {
			String algorithm = "MD5";
			try {
				return MessageDigest.getInstance(algorithm);
			} catch (NoSuchAlgorithmException e) {
				throw new IllegalArgumentException("No such algorithm [" + algorithm + "]");
			}
		}

		protected String mergePasswordAndSalt(String password, Object salt, boolean strict) {
			if (password == null) {
				password = "";
			}
			if (strict && (salt != null)) {
				if ((salt.toString().lastIndexOf("{") != -1) || (salt.toString().lastIndexOf("}") != -1)) {
					throw new IllegalArgumentException("Cannot use { or } in salt.toString()");
				}
			}
			if ((salt == null) || "".equals(salt)) {
				return password;
			} else {
				return password + "{" + salt.toString() + "}";
			}
		}
}
