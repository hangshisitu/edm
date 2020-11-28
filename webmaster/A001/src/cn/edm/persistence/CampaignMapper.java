package cn.edm.persistence;

import cn.edm.domain.Campaign;



public interface CampaignMapper {
	
	Campaign selectById(Integer campaignId);
	
	long selectTaskNumById(Integer campaignId);
	
	String selectLatestDeliveryTime(Integer campaignId);
	
	public void delCampaignByUserId(String userId);
}
