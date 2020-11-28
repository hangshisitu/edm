package cn.edm.exception;

import org.springframework.security.core.AuthenticationException;

public class UserStatusException extends AuthenticationException{

	private static final long serialVersionUID = 7331196320917830918L;

	public UserStatusException(String msg) {
		super(msg);
		// TODO Auto-generated constructor stub
	}
	
	public UserStatusException(String msg, Throwable cause) {
		super(msg, cause);
	}


}
