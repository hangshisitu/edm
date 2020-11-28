package cn.edm.exception;

import org.springframework.security.core.AuthenticationException;

public class UserNameOrPasswordException extends AuthenticationException{
	
	private static final long serialVersionUID = -6022981200733235822L;

	public UserNameOrPasswordException(String msg) {
		super(msg);
		// TODO Auto-generated constructor stub
	}
	
	public UserNameOrPasswordException(String msg, Throwable cause) {
		super(msg, cause);
	}


}
