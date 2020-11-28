package cn.edm.domain;

import java.util.Date;

/**
 * 过滤器.
 * props(属性过滤, 参考cn.edm.constants.Filters. eg. email:end=qq.com&gender:eq=男&birthday:range=1990-01-01,1995-12-31)
 * 
 * @author yjli
 */
public class Filter {

	private Integer filterId;
	private Integer corpId;
	private String userId;
	private String filterName;
	private String filterDesc;
	private String props;
	private Date createTime;
	private Date modifyTime;
	
	public Integer getFilterId() {
		return filterId;
	}

	public void setFilterId(Integer filterId) {
		this.filterId = filterId;
	}

	public Integer getCorpId() {
		return corpId;
	}

	public void setCorpId(Integer corpId) {
		this.corpId = corpId;
	}

	public String getUserId() {
		return userId;
	}

	public void setUserId(String userId) {
		this.userId = userId;
	}

	public String getFilterName() {
		return filterName;
	}

	public void setFilterName(String filterName) {
		this.filterName = filterName;
	}

	public String getFilterDesc() {
		return filterDesc;
	}

	public void setFilterDesc(String filterDesc) {
		this.filterDesc = filterDesc;
	}

	public String getProps() {
		return props;
	}

	public void setProps(String props) {
		this.props = props;
	}

	public Date getCreateTime() {
		return createTime;
	}

	public void setCreateTime(Date createTime) {
		this.createTime = createTime;
	}

	public Date getModifyTime() {
		return modifyTime;
	}

	public void setModifyTime(Date modifyTime) {
		this.modifyTime = modifyTime;
	}
}
