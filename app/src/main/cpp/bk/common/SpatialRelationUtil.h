/*
 * SpatialRelationUtil.h
 *
 *  Created on: 2019年1月17日
 *      Author: chengang
 */

#ifndef COMMON_SPATIALRELATIONUTIL_H_
#define COMMON_SPATIALRELATIONUTIL_H_
#include <vector>
#include <SkPoint.h>
#include <math.h>

struct SpatialRelationUtil{
	/**
	 * 返回一个点是否在一个多边形区域内
	 *
	 * @param mPoints 多边形坐标点列表
	 * @param point   待判断点
	 * @return true 多边形包含这个点,false 多边形未包含这个点。
	 */
	static bool isPolygonContainsPoint(const std::vector<SkPoint>& points,
			const SkPoint &point) {
		int nCross = 0;
		for (int i = 0; i < points.size(); i++) {
			SkPoint p1 = points.at(i);
			SkPoint p2 = points.at((i + 1) % points.size());
			// 取多边形任意一个边,做点point的水平延长线,求解与当前边的交点个数
			// p1p2是水平线段,要么没有交点,要么有无限个交点
			if (p1.fY == p2.fY)
				continue;
			// point 在p1p2 底部 --> 无交点
			if (point.fY < std::min(p1.fY, p2.fY))
				continue;
			// point 在p1p2 顶部 --> 无交点
			if (point.fY >= std::max(p1.fY, p2.fY))
				continue;
			// 求解 point点水平线与当前p1p2边的交点的 X 坐标
			double x = (point.fY - p1.fY) * (p2.fX - p1.fX) / (p2.fY - p1.fY) + p1.fX;
			if (x > point.fX) // 当x=point.x时,说明point在p1p2线段上
				nCross++; // 只统计单边交点
		}
		// 单边交点为偶数，点在多边形之外 ---
		return (nCross % 2 == 1);
	}

	static bool isPolygonContainsPoint(const SkPoint *points, const int count, const SkPoint &point) {
			int nCross = 0;
			for (int i = 0; i < count; i++) {
				SkPoint p1 = points[i];
				SkPoint p2 = points[((i + 1) % count)];
				// 取多边形任意一个边,做点point的水平延长线,求解与当前边的交点个数
				// p1p2是水平线段,要么没有交点,要么有无限个交点
				if (p1.fY == p2.fY)
					continue;
				// point 在p1p2 底部 --> 无交点
				if (point.fY < std::min(p1.fY, p2.fY))
					continue;
				// point 在p1p2 顶部 --> 无交点
				if (point.fY >= std::max(p1.fY, p2.fY))
					continue;
				// 求解 point点水平线与当前p1p2边的交点的 X 坐标
				double x = (point.fY - p1.fY) * (p2.fX - p1.fX) / (p2.fY - p1.fY) + p1.fX;
				if (x > point.fX) // 当x=point.x时,说明point在p1p2线段上
					nCross++; // 只统计单边交点
			}
			// 单边交点为偶数，点在多边形之外 ---
			return (nCross % 2 == 1);
		}

	/**
	 * 返回一个点是否在一个多边形边界上
	 *
	 * @param mPoints 多边形坐标点列表
	 * @param point   待判断点
	 * @return true 点在多边形边上,false 点不在多边形边上。
	 */
	static bool isPointInPolygonBoundary(const std::vector<SkPoint> &points,
			const SkPoint &point) {
		for (int i = 0; i < points.size(); i++) {
			SkPoint p1 = points.at(i);
			SkPoint p2 = points.at((i + 1) % points.size());
			// 取多边形任意一个边,做点point的水平延长线,求解与当前边的交点个数
			// point 在p1p2 底部 --> 无交点
			if (point.fY < std::min(p1.fY, p2.fY))
				continue;
			// point 在p1p2 顶部 --> 无交点
			if (point.fY > std::max(p1.fY, p2.fY))
				continue;

			// p1p2是水平线段,要么没有交点,要么有无限个交点
			if (p1.fY == p2.fY) {
				double minX = std::min(p1.fX, p2.fX);
				double maxX = std::max(p1.fX, p2.fX);
				// point在水平线段p1p2上,直接return true
				if ((point.fY == p1.fY) && (point.fX >= minX && point.fX <= maxX)) {
					return true;
				}
			} else { // 求解交点
				double x = (point.fY - p1.fY) * (p2.fX - p1.fX) / (p2.fY - p1.fY)
						+ p1.fX;
				if (x == point.fX) // 当x=point.x时,说明point在p1p2线段上
					return true;
			}
		}
		return false;
	}
};
#endif /* COMMON_SPATIALRELATIONUTIL_H_ */
